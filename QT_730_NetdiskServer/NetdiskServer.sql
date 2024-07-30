
create database if not exists NetdiskServer;

create table if not exists user_info(
  id int primary key auto_increment,
  name varchar(32) not null,
  pwd varchar(32) not null,
  online tinyint(1) default 0
  
);

create table if not exists friend(
  id int primary key auto_increment,
  user_id int not null,
  friend_id int not null,
  foreign key(user_id) references user_info(id),
  foreign key(friend_id) references user_info(id)
);

insert into user_info(name,pwd)
values
('weihong','171223'),
('shisan','180110');

insert into friend(user_id,friend_id) values(1,2);

select * from user_info where name = 'zhangsan' and pwd = 'zhangsan';


update user_info set online = 1 where name = '';
