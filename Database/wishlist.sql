--
-- Table structure for table `wishlist`
--

DROP TABLE IF EXISTS `wishlist`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `wishlist` (
  `itemowner` decimal(20,0) default NULL,
  `itemhead` int(11) default '0',
  `itemdata` int(11) default '0',
  `slot` int(11) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

