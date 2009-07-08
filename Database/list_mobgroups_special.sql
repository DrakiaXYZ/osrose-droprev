SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for list_mobgroups_special
-- ----------------------------
CREATE TABLE `list_mobgroups_special` (
`id` int(11) NOT NULL auto_increment,
`map` int(11) NOT NULL,
`x` float NOT NULL,
`y` float NOT NULL,
`range` int(11) NOT NULL,
`respawntime` int(11) NOT NULL,
`limit` int(11) NOT NULL,
`tacticalpoints` int(11) NOT NULL,
`moblist` varchar(1024) default NULL,
`daynight` int(11) NOT NULL default 0,
`isactive` int(11) NOT NULL default 1,
`whatisit` varchar(50) default NULL,
PRIMARY KEY  (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5514.00, 5363.00, 10, 20, 5, 200, '3012,1,0|3012,1,0|3012,1,0|3013,1,0|3013,1,0|3013,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5567.00, 5365.00, 10, 20, 9, 200, '3012,1,0|3012,1,0|3012,1,0|3013,1,0|3013,1,0|3013,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5706.00, 5362.00, 10, 20, 8, 200, '3012,1,0|3012,1,0|3012,1,0|3013,1,0|3013,1,0|3013,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5704.00, 5285.00, 10, 20, 5, 200, '3012,1,0|3012,1,0|3012,1,0|3013,1,0|3013,1,0|3013,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5706.00, 5203.00, 10, 20, 8, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5695.00, 5098.00, 10, 20, 5, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5565.00, 5075.00, 10, 20, 6, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5459.00, 5075.00, 10, 20, 7, 200, '3012,1,0|3012,1,0|3012,1,0|3013,1,0|3013,1,0|3013,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5381.00, 5675.00, 10, 20, 8, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5385.00, 5146.00, 10, 20, 5, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5384.00, 5276.00, 10, 20, 7, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5395.00, 5364.00, 10, 20, 7, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5481.00, 5364.00, 10, 20, 8, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5283.00, 5495.00, 10, 20, 5, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5390.00, 5518.00, 10, 20, 6, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5504.00, 5521.00, 10, 20, 6, 200, '3012,1,0|3012,1,0|3012,1,0|3013,1,0|3013,1,0|3013,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5532.00, 5472.00, 10, 20, 9, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5624.00, 5485.00, 10, 20, 5, 200, '3012,1,0|3012,1,0|3012,1,0|3013,1,0|3013,1,0|3013,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5714.00, 5498.00, 10, 20, 9, 200, '3012,1,0|3012,1,0|3012,1,0|3013,1,0|3013,1,0|3013,1,0',2,0,'Halloween_2008_JP');
INSERT INTO `list_mobgroups_special` VALUES (0, 2, 5788.00, 5498.00, 10, 20, 5, 200, '3009,1,0|3009,1,0|3009,1,0|3010,1,0|3010,1,0|3010,1,0',2,0,'Halloween_2008_JP');
