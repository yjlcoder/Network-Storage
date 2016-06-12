-- MySQL dump 10.14  Distrib 5.5.41-MariaDB, for Linux (x86_64)
--
-- Host: localhost    Database: driver
-- ------------------------------------------------------
-- Server version	5.5.41-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES gbk */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `file`
--

DROP TABLE IF EXISTS `file`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `file` (
  `userid` int(11) NOT NULL,
  `virtualpath` varchar(190) NOT NULL,
  `md5` char(32) DEFAULT NULL,
  PRIMARY KEY (`userid`,`virtualpath`),
  KEY `md5` (`md5`),
  CONSTRAINT `file_ibfk_1` FOREIGN KEY (`userid`) REFERENCES `user` (`id`),
  CONSTRAINT `file_ibfk_2` FOREIGN KEY (`md5`) REFERENCES `status` (`md5`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `file`
--

LOCK TABLES `file` WRITE;
/*!40000 ALTER TABLE `file` DISABLE KEYS */;
INSERT INTO `file` VALUES (1,'/',NULL),(1,'/你好/',NULL),(2,'/',NULL),(2,'/2015sitp/',NULL),(2,'/2015sitp/2015sitp/',NULL),(2,'/2015sitp/2015sitp/需要填写提交的材料/',NULL),(2,'/2015sitp/sitp10协议书（6.9晚交3份，务必认真阅读说明后填写）/',NULL),(2,'/test/',NULL),(2,'/test/test/',NULL),(2,'/test/计算机组成原理作业/',NULL),(2,'/2015sitp/2015sitp/需要填写提交的材料/电信学院创新项目信息总表.xlsx','1dc7b74c40e2a286be9ab9d05d2a80b6'),(2,'/test/计算机组成原理作业/ex5.zip','23e50d25ad52477749879fdeef7de916'),(2,'/2015sitp/2015sitp/sitp10+1352427+李泽霖+18521590448.rar','2aba48ce017a8639edd6cf60bbe2611a'),(2,'/2015sitp/sitp10协议书（6.9晚交3份，务必认真阅读说明后填写）/李泽霖+教室使用情况查询app“上自习吧”的构想与实现.doc','442493142961c65854328c53edaeb79c'),(2,'/test/计算机组成原理作业/ex9.zip','51de27cc0d7d25e142e744cb199b7f98'),(2,'/test/计算机组成原理作业/ex2.zip','552bf16c4795cb7e43fd9016be5f4132'),(2,'/2015sitp/2015sitp/需要填写提交的材料/sitp10+1352427+李泽霖+立项申请书.doc','60f7da9f73678c28e236bd625587bb7a'),(2,'/2015sitp/sitp10协议书（6.9晚交3份，务必认真阅读说明后填写）/协议书填写说明.docx','6509470f6f7f491e6bf051263304b04c'),(2,'/test/计算机组成原理作业/ex4.zip','6e7dbc6fb0b93f0220a9a7957fa68ef5'),(2,'/2015sitp/2015sitp/需要填写提交的材料/sitp10+1352427+李泽霖+评审统计表.xls','74564e9dba2b7861334b13ae7caea89c'),(2,'/c8177f3e6709c93d7a33956f993df8.jpg','8c5e9c902c89c57e20f449ae3fd6f283'),(2,'/2015sitp/2015sitp/2015sitp申报说明.docx','9ba722f6c737a2bf63799874aa7f8ac4'),(2,'/2015sitp/电信学院SITP10报销注意事项.docx','d04c36e6b9f94d2913c5203d1fe53360'),(2,'/2015sitp/2015sitp/sitp10+1352427+李泽霖+18521590448.pptx','e915214191c015eccd347bbfb389fd63');
/*!40000 ALTER TABLE `file` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `file_links`
--

DROP TABLE IF EXISTS `file_links`;
/*!50001 DROP VIEW IF EXISTS `file_links`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `file_links` (
  `file_md5` tinyint NOT NULL,
  `file_link_num` tinyint NOT NULL
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `log`
--

DROP TABLE IF EXISTS `log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log` (
  `logid` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) DEFAULT NULL,
  `time` datetime NOT NULL,
  `status` tinyint(1) NOT NULL,
  PRIMARY KEY (`logid`),
  KEY `userid` (`userid`),
  CONSTRAINT `log_ibfk_1` FOREIGN KEY (`userid`) REFERENCES `user` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `log`
--

LOCK TABLES `log` WRITE;
/*!40000 ALTER TABLE `log` DISABLE KEYS */;
/*!40000 ALTER TABLE `log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `status`
--

DROP TABLE IF EXISTS `status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `status` (
  `md5` char(32) NOT NULL,
  `status` int(11) NOT NULL,
  PRIMARY KEY (`md5`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `status`
--

LOCK TABLES `status` WRITE;
/*!40000 ALTER TABLE `status` DISABLE KEYS */;
INSERT INTO `status` VALUES ('1dc7b74c40e2a286be9ab9d05d2a80b6',1),('23e50d25ad52477749879fdeef7de916',1),('2aba48ce017a8639edd6cf60bbe2611a',1),('442493142961c65854328c53edaeb79c',1),('51de27cc0d7d25e142e744cb199b7f98',1),('552bf16c4795cb7e43fd9016be5f4132',1),('60f7da9f73678c28e236bd625587bb7a',1),('6509470f6f7f491e6bf051263304b04c',1),('6e7dbc6fb0b93f0220a9a7957fa68ef5',1),('74564e9dba2b7861334b13ae7caea89c',1),('8c5e9c902c89c57e20f449ae3fd6f283',1),('9ba722f6c737a2bf63799874aa7f8ac4',1),('d04c36e6b9f94d2913c5203d1fe53360',1),('e915214191c015eccd347bbfb389fd63',1);
/*!40000 ALTER TABLE `status` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(128) NOT NULL,
  `password` char(40) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ix_user_username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES (1,'LovingYoung','bd680b312274ef9faab01bb54a2056206d8b4cc9'),(2,'clw00','6367c48dd193d56ea7b0baad25b19455e529f5ee');
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Final view structure for view `file_links`
--

/*!50001 DROP TABLE IF EXISTS `file_links`*/;
/*!50001 DROP VIEW IF EXISTS `file_links`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = gbk */;
/*!50001 SET character_set_results     = gbk */;
/*!50001 SET collation_connection      = gbk_chinese_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `file_links` AS select `file`.`md5` AS `file_md5`,count(`file`.`virtualpath`) AS `file_link_num` from `file` group by `file`.`md5` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2016-06-12 21:35:37
