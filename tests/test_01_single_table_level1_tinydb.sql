create database testdb;
use database testdb;
.read dataset_small/create.sqllite.sql
.read dataset_small/book.sql
.read dataset_small/customer.sql
.read dataset_small/orders.sql
.read dataset_small/publisher.sql
select * from publisher;
select * from book;
select * from customer;
select * from orders;
.quit
