.header on
CREATE TABLE customer (
  id int(10) NOT NULL,
  name varchar(25) NOT NULL,
  gender varchar(1),
  PRIMARY KEY  (id)
);
.read dataset_small/customer.sql
select * from customer;
.quit
