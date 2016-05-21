set names gbk;
drop database if exists test;
create database test;
use test;

drop table if exists dbtest5;
create table dbtest5 (
dbtest_pid int(5),
dbtest_time datetime,
dbtest_str varchar(10000) not null,
primary key(dbtest_pid, dbtest_time)
)engine = InnoDB;

