# Generated by CacheToSQL 2 on 12/12/2011 11:46:15 AM (UTC)
# Source: cacheChrBloodlines
# File: 3200002.cache2

DROP TABLE IF EXISTS extChrBloodlines;
CREATE TABLE extChrBloodlines (
    bloodlineID INT,
    bloodlineNameID INT,
    descriptionID INT,
    dataID INT,
    PRIMARY KEY (bloodlineID),
    INDEX bloodlineNameID (bloodlineNameID),
    INDEX descriptionID (descriptionID),
    INDEX dataID (dataID)
);

INSERT INTO extChrBloodlines (bloodlineID, bloodlineNameID, descriptionID, dataID) VALUES
(1, 59534, 59520, 16533065),
(2, 59533, 59519, 16533064),
(3, 59536, 59522, 16533067),
(4, 59537, 59523, 16533068),
(5, 59541, 59527, 16533072),
(6, 59539, 59525, 16533070),
(7, 59542, 59528, 16533073),
(8, 59544, 59530, 16533075),
(9, 59531, 59517, 16533062),
(10, 59532, 59518, 16533063),
(11, 59535, 59521, 16533066),
(12, 59543, 59529, 16533074),
(13, 59540, 59526, 16533071),
(14, 59538, 59524, 16533069);
