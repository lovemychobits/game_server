
CREATE DATABASE IF NOT EXISTS `game_server`;
USE `game_server`;

CREATE TABLE IF NOT EXISTS `account` (
  `accountid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(32) NOT NULL DEFAULT '',
  `password` varchar(32) DEFAULT '',
  PRIMARY KEY (`accountid`),
  KEY `username` (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `game_zone` (
  `game_zone_id` int(11) unsigned NOT NULL,
  `name` varchar(32) NOT NULL,
  `state` int(10) unsigned NOT NULL DEFAULT '0',
  `is_online` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `zone_desc` varchar(50) DEFAULT NULL,
  `ip` varchar(16) NOT NULL,
  `port` smallint(5) unsigned NOT NULL,
  `online_count` int(10) unsigned NOT NULL DEFAULT '0',
  `center_server_id` INT(10) UNSIGNED NOT NULL DEFAULT '0',
  `update_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`game_zone_id`),
  UNIQUE KEY `IP_PORT_UNIQUE` (`ip`,`port`),
  KEY `NAME_KEY` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='gate通过gameserver上传区服表';

CREATE TABLE IF NOT EXISTS `hash_game_db` (
  `group_id` int(11) NOT NULL,
  `ip` char(16) NOT NULL COMMENT 'can merge into same ip',
  `port` smallint(5) unsigned NOT NULL COMMENT 'can merge into same port',
  `user_name` varchar(36) NOT NULL,
  `password` varchar(36) NOT NULL,
  `db_name` varchar(36) NOT NULL,
  `charset` varchar(36) NOT NULL,
  PRIMARY KEY (`group_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='每个区数据库地址';

CREATE TABLE IF NOT EXISTS `player_zone_id_2_db` (
  `old_playerid_zone_id` int(11) NOT NULL,
  `group_id` int(11) NOT NULL,
  PRIMARY KEY (`old_playerid_zone_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `show_zone_2_game_zone` (
  `show_zone_id` int(11) NOT NULL,
  `game_zone_id` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


