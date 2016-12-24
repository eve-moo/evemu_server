/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2016 The EVEmu Team
    For the latest information visit http://evemu.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:        caytchen
 */

#include "eve-server.h"

#include "imageserver/ImageServer.h"
#include "imageserver/ImageServerConnection.h"
#include "network/URLPageReader.h"

boost::asio::const_buffers_1 ImageServerConnection::_responseOK = boost::asio::buffer("HTTP/1.0 200 OK\r\nContent-Type: image/jpeg\r\n\r\n", 45);
boost::asio::const_buffers_1 ImageServerConnection::_responseNotFound = boost::asio::buffer("HTTP/1.0 404 Not Found\r\n\r\n", 26);
boost::asio::const_buffers_1 ImageServerConnection::_responseRedirectBegin = boost::asio::buffer("HTTP/1.0 301 Moved Permanently\r\nLocation: ", 42);
boost::asio::const_buffers_1 ImageServerConnection::_responseRedirectEnd = boost::asio::buffer("\r\n\r\n", 4);

ImageServerConnection::ImageServerConnection(boost::asio::io_service& io)
: _socket(io) { }

boost::asio::ip::tcp::socket& ImageServerConnection::socket()
{
    return _socket;
}

void ImageServerConnection::Process()
{
    // receive all HTTP headers from the client
    boost::asio::async_read_until(_socket, _buffer, "\r\n\r\n", std::bind(&ImageServerConnection::ProcessHeaders, shared_from_this()));
}

void ImageServerConnection::ProcessHeaders()
{
    std::istream stream(&_buffer);
    std::string request;

    // every request line ends with \r\n
    std::getline(stream, request, '\r');

    if(!starts_with(request, "GET /"))
    {
        NotFound();
        return;
    }
    request = request.substr(5);

    bool found = false;
    for(uint32 i = 0; i < ImageServer::CategoryCount; i++)
    {
        if(starts_with(request, ImageServer::Categories[i]))
        {
            found = true;
            _category = ImageServer::Categories[i];
            request = request.substr(strlen(ImageServer::Categories[i]));
            break;
        }
    }
    if(!found)
    {
        NotFound();
        return;
    }

    if(!starts_with(request, "/"))
    {
        NotFound();
        return;
    }
    request = request.substr(1);

    int del = request.find_first_of('_');
    if(del == (int)std::string::npos)
    {
        NotFound();
        return;
    }

    // might have some extra data but atoi shouldn't care
    std::string idStr = request.substr(0, del);
    std::string sizeStr = request.substr(del + 1);
    _id = atoi(idStr.c_str());
    _size = atoi(sizeStr.c_str());

    _imageData = ImageServer::getImage(_category, _id, _size);
    if(!_imageData)
    {
        // If were have an id that is less than our starting entity ID forward the request to the official image server.
        if(_id < 80000000)
        {
            // Get the file to request.
            std::string page = ImageServer::getFilePath(_category, _id, _size, false);
            std::vector<std::string> header;
            std::vector<char> *data = new std::vector<char>();
            if(getPageFromURL("image.eveonline.com", 80, page, header, *data))
            {
                std::string path(ImageServer::getFilePath(_category, _id, _size));
                FILE * fp = fopen(path.c_str(), "wb");
                fwrite(&((*data)[0]), 1, data->size(), fp);
                fclose(fp);
                _imageData.reset(data);
            }
            else
            {
                SysLog::Warning("ImageServer", "Unable to find image for %s ID: %u Size: %u", _category.c_str(), _id, _size);
                NotFound();
                return;
            }
        }
        else
        {
            // ID is greater than static data allowed.
            NotFound();
            return;
        }
    }

    // first we have to send the responseOK, then our actual result
    boost::asio::async_write(_socket, _responseOK, boost::asio::transfer_all(), std::bind(&ImageServerConnection::SendImage, shared_from_this()));
}

void ImageServerConnection::SendImage()
{
    boost::asio::async_write(_socket, boost::asio::buffer(*_imageData, _imageData->size()), boost::asio::transfer_all(), std::bind(&ImageServerConnection::Close, shared_from_this()));
}

void ImageServerConnection::NotFound()
{
    boost::asio::async_write(_socket, _responseNotFound, boost::asio::transfer_all(), std::bind(&ImageServerConnection::Close, shared_from_this()));
}

void ImageServerConnection::Close()
{
    _socket.close();
}

bool ImageServerConnection::starts_with(std::string& haystack, const char *const needle)
{
    return haystack.substr(0, strlen(needle)).compare(needle) == 0;
}

std::shared_ptr<ImageServerConnection> ImageServerConnection::create(boost::asio::io_service& io)
{
    return std::shared_ptr<ImageServerConnection>(new ImageServerConnection(io));
}
