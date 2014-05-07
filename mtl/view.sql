create view plan_ok_yesterday as select saddr, eaddr, km, repeat, sdate, stime from plan where statu=2 and intime > current_date -1;
create view plan_error_yesterday as select saddr, eaddr, km, repeat, sdate, stime from plan where statu=11 and intime > current_date -1;
