# Generated by CacheToSQL 2 on 14/12/2011 4:32:21 AM (UTC)

DROP TABLE IF EXISTS extChrAncestries;
CREATE TABLE extChrAncestries (
    ancestryID INT,
    ancestryNameID INT,
    descriptionID INT,
    dataID INT,
    PRIMARY KEY (ancestryID),
    INDEX ancestryNameID (ancestryNameID),
    INDEX descriptionID (descriptionID),
    INDEX dataID (dataID)
);

INSERT INTO extChrAncestries (ancestryID, ancestryNameID, descriptionID, dataID) VALUES
(1, 59507, 59464, 16533052),
(2, 59508, 59465, 16533053),
(3, 59483, 59440, 16533028),
(4, 59484, 59441, 16533029),
(5, 59485, 59442, 16533030),
(6, 59514, 59471, 16533059),
(7, 59499, 59456, 16533044),
(8, 59494, 59451, 16533039),
(9, 59493, 59450, 16533038),
(10, 59498, 59455, 16533043),
(11, 59497, 59454, 16533042),
(12, 59496, 59453, 16533041),
(13, 59489, 59446, 16533034),
(14, 59512, 59469, 16533057),
(15, 59513, 59470, 16533058),
(16, 59490, 59447, 16533035),
(17, 59491, 59448, 16533036),
(18, 59492, 59449, 16533037),
(19, 59502, 59459, 16533047),
(20, 59481, 59438, 16533026),
(21, 59482, 59439, 16533027),
(22, 59503, 59460, 16533048),
(23, 59505, 59462, 16533050),
(24, 59504, 59461, 16533049),
(25, 59473, 59430, 16533018),
(26, 59474, 59431, 16533019),
(27, 59475, 59432, 16533020),
(28, 59476, 59433, 16533021),
(29, 59477, 59434, 16533022),
(30, 59478, 59435, 16533023),
(31, 59500, 59457, 16533045),
(32, 59501, 59458, 16533046),
(33, 59495, 59452, 16533040),
(34, 59486, 59443, 16533031),
(35, 59487, 59444, 16533032),
(36, 59488, 59445, 16533033),
(37, 59509, 59466, 16533054),
(38, 59510, 59467, 16533055),
(39, 59511, 59468, 16533056),
(40, 59479, 59436, 16533024),
(41, 59480, 59437, 16533025),
(42, 59506, 59463, 16533051);

