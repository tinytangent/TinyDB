create database testdb;
use database testdb;
CREATE TABLE customer ( id int(10) NOT NULL, name varchar(25) NOT NULL, gender varchar(1));
.read dataset_small/customer.sql
select * from customer;
.quit
