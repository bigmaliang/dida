-- 处理用户的“对结果不满意”，
-- 所有这种期盼立即被放到 expect 里，
-- 所有 plan 入库后遍历该表，满足的话插入 meet 表
CREATE TABLE expect (
	id SERIAL,
    mid int NOT NULL DEFAULT 0, --member id
    mname varchar(256) NOT NULL DEFAULT '',
    pid int NOT NULL DEFAULT 0, --plan id
    statu smallint NOT NULL DEFAULT 0,    --0 ok, 1 pause, 2 delete
    addrtype smallint NOT NULL DEFAULT 1, --1 phone, 2 email, 3 all
    --toaddr varchar(128) NOT NULL DEFAULT '',
    gotime smallint NOT NULL DEFAULT 0,  --0 immediatly, 1 day, 2 week

    dad smallint NOT NULL DEFAULT 0,   --0 person, 1 car
    sgeo point NOT NULL DEFAULT '(0,0)',
    rect box NOT NULL DEFAULT '((0,0), (1,1))',
    km numeric NOT NULL DEFAULT 0.1,
    repeat smallint NOT NULL DEFAULT 0, --0 no, 1 day, 2 week
    sdate varchar(16) NOT NULL DEFAULT '2011-11-11', --or 1,3,5
    stime time NOT NULL DEFAULT '08:00:00',
    
    intime timestamp DEFAULT now()
);
CREATE INDEX expect_index ON expect (mid, pid, addrtype);

CREATE FUNCTION merge_expect(cmid int, cmname character varying, cpid int, caddrtype int, cstatu int, cgotime int, cdad int, csgeo point, crect box, ckm numeric, crepeat int, csdate character varying, cstime time) RETURNS VOID AS
$$
BEGIN
    LOOP
        -- first try to update the key
        UPDATE expect SET statu=cstatu, gotime=cgotime, dad=cdad, sgeo=csgeo, rect=crect, km=ckm, repeat=crepeat, sdate=csdate, stime=cstime WHERE mid=cmid AND pid=cpid AND addrtype=caddrtype;
        IF found THEN
            RETURN;
        END IF;
        -- not there, so try to insert the key
        -- if someone else inserts the same key concurrently,
        -- we could get a unique-key failure
        BEGIN
            INSERT INTO expect(mid, mname, pid, addrtype, statu, gotime, dad, sgeo, rect, km, repeat, sdate, stime) VALUES (cmid, cmname, cpid, caddrtype, cstatu, cgotime, cdad, csgeo, crect, ckm, crepeat, csdate, cstime);
            RETURN;
        EXCEPTION WHEN unique_violation THEN
            -- do nothing, and loop to try the UPDATE again
        END;
    END LOOP;
END
$$
LANGUAGE plpgsql;

CREATE TABLE meet (
    id SERIAL,
    eid int NOT NULL DEFAULT 0, --expect id
    pid int NOT NULL DEFAULT 0, --which plan meet the expect
    intime timestamp DEFAULT now()
);
CREATE INDEX meet_index ON meet (eid, pid);
