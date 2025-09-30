-- phpMyAdmin SQL Dump
-- version 5.2.2
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Sep 30, 2025 at 01:01 AM
-- Server version: 8.0.42-cll-lve
-- PHP Version: 8.4.10

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `dwljzndr_tank`
--

-- --------------------------------------------------------

--
-- Table structure for table `alert_settings`
--

CREATE TABLE `alert_settings` (
  `id` int NOT NULL,
  `tank_id` int NOT NULL,
  `alert_type` enum('low_level','high_level','offline') COLLATE utf8mb4_unicode_ci NOT NULL,
  `threshold` int NOT NULL,
  `is_enabled` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` datetime DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `alert_settings`
--

INSERT INTO `alert_settings` (`id`, `tank_id`, `alert_type`, `threshold`, `is_enabled`, `created_at`) VALUES
(1, 1, 'low_level', 20, 1, '2025-09-23 14:46:06'),
(2, 1, 'high_level', 180, 1, '2025-09-23 14:46:06'),
(3, 1, 'offline', 300, 1, '2025-09-23 14:46:06');

-- --------------------------------------------------------

--
-- Table structure for table `api_keys`
--

CREATE TABLE `api_keys` (
  `id` int NOT NULL,
  `api_key` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `tank_id` int DEFAULT NULL,
  `description` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `is_active` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` datetime DEFAULT CURRENT_TIMESTAMP,
  `last_used` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `api_keys`
--

INSERT INTO `api_keys` (`id`, `api_key`, `tank_id`, `description`, `is_active`, `created_at`, `last_used`) VALUES
(1, 'YOUR_SECURE_API_KEY_HERE', 1, 'Default API key for tank monitoring', 1, '2025-09-23 14:46:06', NULL);

-- --------------------------------------------------------

--
-- Stand-in structure for view `hourly_averages`
-- (See below for the actual view)
--
CREATE TABLE `hourly_averages` (
`avg_level` decimal(14,4)
,`avg_percentage` decimal(14,4)
,`hour_start` varchar(24)
,`max_level` int
,`min_level` int
,`reading_count` bigint
,`tank_id` int
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `latest_tank_readings`
-- (See below for the actual view)
--
CREATE TABLE `latest_tank_readings` (
`level` int
,`max_depth` int
,`min_depth` int
,`percentage` int
,`status` varchar(6)
,`status_class` varchar(7)
,`tank_id` int
,`tank_name` varchar(100)
,`timestamp` datetime
);

-- --------------------------------------------------------

--
-- Table structure for table `system_logs`
--

CREATE TABLE `system_logs` (
  `id` int NOT NULL,
  `level` enum('INFO','WARNING','ERROR','CRITICAL') COLLATE utf8mb4_unicode_ci NOT NULL,
  `message` text COLLATE utf8mb4_unicode_ci NOT NULL,
  `source` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `created_at` datetime DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `tank_config`
--

CREATE TABLE `tank_config` (
  `id` int NOT NULL,
  `tank_id` int NOT NULL,
  `tank_name` varchar(100) COLLATE utf8mb4_unicode_ci NOT NULL,
  `max_depth` int NOT NULL DEFAULT '200',
  `min_depth` int NOT NULL DEFAULT '20',
  `location` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `description` text COLLATE utf8mb4_unicode_ci,
  `is_active` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` datetime DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `tank_config`
--

INSERT INTO `tank_config` (`id`, `tank_id`, `tank_name`, `max_depth`, `min_depth`, `location`, `description`, `is_active`, `created_at`, `updated_at`) VALUES
(1, 1, 'Main Water Tank', 200, 20, 'Ground Floor', 'Primary water storage tank with ultrasonic monitoring', 1, '2025-09-23 14:46:06', '2025-09-23 14:46:06');

-- --------------------------------------------------------

--
-- Table structure for table `tank_data`
--

CREATE TABLE `tank_data` (
  `id` int NOT NULL,
  `tank_id` int NOT NULL,
  `level` int NOT NULL,
  `percentage` int NOT NULL,
  `timestamp` datetime DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `tank_data`
--

INSERT INTO `tank_data` (`id`, `tank_id`, `level`, `percentage`, `timestamp`) VALUES
(1, 1, 20, 20, '2025-09-28 00:06:52'),
(2, 1, 20, 20, '2025-09-28 00:07:21'),
(407, 1, 150, 75, '2025-09-28 00:39:21'),
(408, 1, 150, 75, '2025-09-28 00:39:51'),
(409, 1, 150, 75, '2025-09-28 00:41:44'),
(410, 1, 167, 84, '2025-09-28 01:18:40'),
(411, 1, 167, 84, '2025-09-28 01:20:32');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `alert_settings`
--
ALTER TABLE `alert_settings`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `uk_tank_alert_type` (`tank_id`,`alert_type`),
  ADD KEY `idx_tank_id` (`tank_id`);

--
-- Indexes for table `api_keys`
--
ALTER TABLE `api_keys`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `uk_api_key` (`api_key`),
  ADD KEY `idx_tank_id` (`tank_id`),
  ADD KEY `idx_api_keys_active` (`is_active`,`api_key`);

--
-- Indexes for table `system_logs`
--
ALTER TABLE `system_logs`
  ADD PRIMARY KEY (`id`),
  ADD KEY `idx_level` (`level`),
  ADD KEY `idx_created_at` (`created_at`);

--
-- Indexes for table `tank_config`
--
ALTER TABLE `tank_config`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `uk_tank_id` (`tank_id`);

--
-- Indexes for table `tank_data`
--
ALTER TABLE `tank_data`
  ADD PRIMARY KEY (`id`),
  ADD KEY `idx_tank_timestamp` (`tank_id`,`timestamp`),
  ADD KEY `idx_timestamp` (`timestamp`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `alert_settings`
--
ALTER TABLE `alert_settings`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `api_keys`
--
ALTER TABLE `api_keys`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `system_logs`
--
ALTER TABLE `system_logs`
  MODIFY `id` int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `tank_config`
--
ALTER TABLE `tank_config`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `tank_data`
--
ALTER TABLE `tank_data`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=412;

-- --------------------------------------------------------

--
-- Structure for view `hourly_averages`
--
DROP TABLE IF EXISTS `hourly_averages`;

CREATE ALGORITHM=UNDEFINED DEFINER=`dwljzndr`@`localhost` SQL SECURITY DEFINER VIEW `hourly_averages`  AS SELECT `tank_data`.`tank_id` AS `tank_id`, date_format(`tank_data`.`timestamp`,'%Y-%m-%d %H:00:00') AS `hour_start`, avg(`tank_data`.`level`) AS `avg_level`, avg(`tank_data`.`percentage`) AS `avg_percentage`, min(`tank_data`.`level`) AS `min_level`, max(`tank_data`.`level`) AS `max_level`, count(0) AS `reading_count` FROM `tank_data` WHERE (`tank_data`.`timestamp` >= (now() - interval 24 hour)) GROUP BY `tank_data`.`tank_id`, date_format(`tank_data`.`timestamp`,'%Y-%m-%d %H:00:00') ORDER BY `tank_data`.`tank_id` ASC, date_format(`tank_data`.`timestamp`,'%Y-%m-%d %H:00:00') ASC ;

-- --------------------------------------------------------

--
-- Structure for view `latest_tank_readings`
--
DROP TABLE IF EXISTS `latest_tank_readings`;

CREATE ALGORITHM=UNDEFINED DEFINER=`dwljzndr`@`localhost` SQL SECURITY DEFINER VIEW `latest_tank_readings`  AS SELECT `td`.`tank_id` AS `tank_id`, `tc`.`tank_name` AS `tank_name`, `tc`.`max_depth` AS `max_depth`, `tc`.`min_depth` AS `min_depth`, `td`.`level` AS `level`, `td`.`percentage` AS `percentage`, `td`.`timestamp` AS `timestamp`, (case when (`td`.`percentage` >= 80) then 'Full' when (`td`.`percentage` >= 50) then 'High' when (`td`.`percentage` >= 20) then 'Medium' when (`td`.`percentage` > 0) then 'Low' else 'Empty' end) AS `status`, (case when (`td`.`percentage` <= 20) then 'warning' when (`td`.`percentage` <= 10) then 'danger' else 'normal' end) AS `status_class` FROM (`tank_data` `td` join `tank_config` `tc` on((`td`.`tank_id` = `tc`.`tank_id`))) WHERE (`td`.`timestamp` = (select max(`td2`.`timestamp`) from `tank_data` `td2` where (`td2`.`tank_id` = `td`.`tank_id`))) ;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
