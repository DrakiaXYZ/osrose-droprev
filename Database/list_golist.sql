/*
MySQL Data Transfer
Source Host: localhost
Source Database: osrose
Target Host: localhost
Target Database: osrose
Date: 3/21/2011 1:35:46 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for list_golist
-- ----------------------------
CREATE TABLE `list_golist` (
  `id` int(11) NOT NULL auto_increment,
  `loc` int(11) NOT NULL,
  `mapname` varchar(50) character set utf8 NOT NULL,
  `lvlmin` int(11) NOT NULL default '0',
  `lvlmax` int(11) NOT NULL default '250',
  `map` int(11) NOT NULL,
  `locx` int(11) NOT NULL,
  `locy` int(11) NOT NULL,
  `isactive` int(11) NOT NULL,
  `description` varchar(50) character set utf8 default NULL,
  PRIMARY KEY  (`id`,`loc`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `list_golist` VALUES ('1', '1', 'Adventure Plains', '0', '250', '22', '5800', '5200', '1', null);
INSERT INTO `list_golist` VALUES ('2', '2', 'Canyon City of Zant', '0', '250', '1', '5240', '5192', '1', null);
INSERT INTO `list_golist` VALUES ('3', '3', 'Junon Polis', '0', '250', '2', '5514', '5247', '1', null);
INSERT INTO `list_golist` VALUES ('4', '4', 'Magic City of Eucar - Luna', '50', '250', '51', '5357', '5013', '1', null);
INSERT INTO `list_golist` VALUES ('5', '5', 'Xita Refuge - Eldeon', '120', '250', '61', '5434', '4569', '1', null);
INSERT INTO `list_golist` VALUES ('6', '6', 'Training Grounds', '0', '250', '6', '5199', '5280', '1', null);
INSERT INTO `list_golist` VALUES ('7', '7', 'Luna Clan Field', '120', '250', '59', '5095', '5128', '1', null);
INSERT INTO `list_golist` VALUES ('8', '8', 'Lions Plains', '0', '250', '8', '5160', '5080', '1', null);
INSERT INTO `list_golist` VALUES ('9', '9', 'Desert of the Dead', '0', '250', '29', '5093', '5144', '1', null);
INSERT INTO `list_golist` VALUES ('10', '10', 'Sikuku Underground Prison', '160', '250', '65', '5485', '5285', '1', null);
INSERT INTO `list_golist` VALUES ('11', '11', 'Oro', '180', '250', '72', '5146', '5278', '1', null);
