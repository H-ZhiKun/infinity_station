-- Create tables
/* for example
CREATE TABLE IF NOT EXISTS device_status (
  id serial PRIMARY KEY,
  tag varchar(20) NOT NULL,
  details_json jsonb,
  created_time timestamp,
  updated_time timestamp
);
1 表名与字段名均使用小写定义。
2 id为主键必须serial PRIMARY KEY修饰
3 created_time和updated_time也是必要字段，用timestamp修饰
3 id与created_time中间插入自定义业务字段。
4 按需为字段添加索引，如下
  CREATE INDEX idx_device_status_created_time ON device_status (created_time);
  CREATE INDEX idx_device_status_updated_time ON device_status (updated_time);

表名与字段名无需""包裹，不使用情况下大小写不敏感，所有字符都会转为小写
*/


-- Table structure

CREATE TABLE IF NOT EXISTS device_status (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  tag varchar(20) NOT NULL,
  details_json jsonb,
  created_time timestamp,
  updated_time timestamp
);
-- 添加索引
CREATE INDEX idx_device_status_created_time ON device_status (created_time);

--mvb创建和插入,定位程序查询
CREATE TABLE IF NOT EXISTS mvb_line_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  tag varchar(20) NOT NULL,
  details_json jsonb,
  created_time timestamp,
  updated_time timestamp
);

--locationServer创建和插入
CREATE TABLE IF NOT EXISTS location_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  task_id INT,
  line_id INT,
  station_name varchar(20) ,
  maoduan_name varchar(20) ,
  pole_name    varchar(20) ,
  kilo_meter  FLOAT NOT NULL,
  move_dis FLOAT,
  location_type INT,
  speed FLOAT,
  created_time timestamp,
  updated_time timestamp
);
--jihe——data表
CREATE TABLE IF NOT EXISTS jihe_data (
   id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, --自增
   task_id INT, 
   zig0 FLOAT,
   zig1 FLOAT,
   zig2 FLOAT,
   zig3 FLOAT,
   hei0 FLOAT,
   hei1 FLOAT,
   hei2 FLOAT,
   hei3 FLOAT,
   zig_bc0 FLOAT,
   zig_bc1 FLOAT,
   zig_bc2 FLOAT,
   zig_bc3 FLOAT,
   hei_bc0 FLOAT,
   hei_bc1 FLOAT,
   hei_bc2 FLOAT,
   hei_bc3 FLOAT,
   left_bc_x FLOAT,
   left_bc_y FLOAT,
   right_bc_x FLOAT,
   right_bc_y FLOAT,
   mohao_width0 FLOAT,
   mohao_width1 FLOAT,
   mohao_width2 FLOAT,
   mohao_width3 FLOAT,
   mohao_height0 FLOAT,
   mohao_height1 FLOAT,
   mohao_height2 FLOAT,
   mohao_height3 FLOAT,
   diameter0 FLOAT,
   diameter1 FLOAT,
   diameter2 FLOAT,
   diameter3 FLOAT,
   created_time timestamp,
   updated_time timestamp
);

--压力硬点表
CREATE TABLE IF NOT EXISTS press_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  task_id INT,
  press0 FLOAT,
  press1 FLOAT,
  press2 FLOAT,
  press3 FLOAT,
  press4 FLOAT,
  press5 FLOAT,
  press6 FLOAT,
  press7 FLOAT,
  acc0_x FLOAT,
  acc0_y FLOAT,
  acc0_z FLOAT,
  acc1_x  FLOAT,
  acc1_y  FLOAT,
  acc1_z  FLOAT,
  created_time timestamp,
  updated_time timestamp
);

-- 采集设备表
CREATE TABLE IF NOT EXISTS collect_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  task_id INT,
  vol FLOAT,--网压
  cur FLOAT,--电流
  tem FLOAT,--温度
  hum FLOAT,--湿度
  created_time timestamp,
  updated_time timestamp
);

--红外表
CREATE TABLE IF NOT EXISTS infrared_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  task_id INT,
  max_tem FLOAT,--最大温度
  min_tem FLOAT,--最小温度
  ave_tem FLOAT,--平均温度
  
  created_time timestamp,
  updated_time timestamp
);

--雷达数据表
CREATE TABLE IF NOT EXISTS radar_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  location_time timestamp,
  task_id INT,
  points JSON, -- 存储多个 (x, y)
  created_time timestamp,
  updated_time timestamp
);

--雷达超限表
CREATE TABLE IF NOT EXISTS radar_over_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  location_time_start timestamp,
  location_time_end timestamp,
  task_id INT,
  points JSON, -- 存储多个 (x, y)
  direction varchar(8), -- 超限方向   上下左右
  overrun_time INT,
  point_count INT, -- 超限点数
  created_time timestamp,
  updated_time timestamp
);


--任务信息表
CREATE TABLE IF NOT EXISTS task_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  task_name varchar(255) NOT NULL, -- 任务名
  line_name varchar(64) NOT NULL, -- 线路名
  line_dir INT NOT NULL, -- 线路方向 0:上行 1:下行
  direction INT NOT NULL, -- 0:正向 1:反向
  start_station varchar(64) NOT NULL, -- 起始站
  end_station varchar(64) NOT NULL, -- 终止站
  start_pole varchar(64) NOT NULL, -- 起始杆号
  end_time timestamp, -- 任务结束时间
  created_time timestamp,
  updated_time timestamp
);

--线路信息表
DROP TABLE IF EXISTS line_data CASCADE;
CREATE TABLE IF NOT EXISTS line_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  tag_id varchar(128), -- RFID标签id
  line_name varchar(20) NOT NULL, -- 线路名
  station_name varchar(20) NOT NULL, -- 站区名
  maoduan_name varchar(20), -- 锚段名  可空
  pole_name varchar(20) NOT NULL, -- 杆号
  kilo_meter FLOAT NOT NULL, -- 公里标
  line_dir INT NOT NULL, -- 线路方向 0:上行 1:下行
  span FLOAT, -- 跨距
  structure varchar(64), -- 结构
  station_id INT, -- 站区id
  created_time timestamp,
  updated_time timestamp
);

CREATE TABLE IF NOT EXISTS original_rfid_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  original_tag_id varchar(128), -- RFID标签id(原始)
  tag_id varchar(128), -- RFID标签id(解析后)
  created_time timestamp,
  updated_time timestamp
);

--燃弧数据表

CREATE TABLE IF NOT EXISTS arc_data (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, -- 自增id
  arc_count INT NOT NULL, -- 燃弧次数
  arc_time INT NOT NULL, -- 燃弧持续时间
  arc_pulse INT NOT NULL, -- 燃弧脉冲数
  arc_timestamp BIGINT NOT NULL, -- 用于与老程序交互的时间戳
  arcvideo_path varchar(100) NOT NULL, -- 燃弧视频路径
  created_time timestamp,
  updated_time timestamp,
  arc_device_index varchar(20) NOT NULL, -- 燃弧设备索引
  camera_name varchar(20) NOT NULL -- 摄像头名称
);
