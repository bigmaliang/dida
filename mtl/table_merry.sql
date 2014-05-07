CREATE TABLE city (
    id SERIAL,
    grade smallint NOT NULL DEFAULT 0,
    pid int NOT NULL DEFAULT 0,
    s varchar(64) NOT NULL DEFAULT '',
    geopos point NOT NULL DEFAULT '(0,0)',
    PRIMARY KEY (id)
);


CREATE TABLE member (
    mid int PRIMARY KEY,
    mname varchar(256) NOT NULL DEFAULT '', --equal to userid, in email format
    mnick varchar(256) NOT NULL DEFAULT '',
    msn varchar(256) NOT NULL DEFAULT '',
    mmsn varchar(256) NOT NULL DEFAULT '',
    male smallint NOT NULL DEFAULT 0,
    pass varchar(64) NOT NULL DEFAULT '',
    mpass varchar(64) NOT NULL DEFAULT '',
    statu smallint NOT NULL DEFAULT 0,  --0 normal, 1 pause, 2 delete, 10 spd fresh
    verify smallint NOT NULL DEFAULT 0, --0 fresh, 1 id credit, 2 subed, 4 jiazhao, 90 admin, ..., 100 root
    credit int NOT NULL DEFAULT 0,
    cityid int NOT NULL DEFAULT 0,
    phone varchar(128) NOT NULL DEFAULT '',
    contact varchar(128) NOT NULL DEFAULT '',
    intime timestamp DEFAULT now()
);
CREATE INDEX member_index ON member (male, verify);

CREATE TABLE memberreset (
       mname varchar(256) NOT NULL DEFAULT '',
       rlink varchar(256) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       PRIMARY KEY (mname)
);

CREATE FUNCTION merge_memberreset(e TEXT, r TEXT) RETURNS VOID AS
$$
BEGIN
    LOOP
        -- first try to update the key
        UPDATE memberreset SET rlink = r WHERE mname = e;
        IF found THEN
            RETURN;
        END IF;
        -- not there, so try to insert the key
        -- if someone else inserts the same key concurrently,
        -- we could get a unique-key failure
        BEGIN
            INSERT INTO memberreset(mname, rlink) VALUES (e, r);
            RETURN;
        EXCEPTION WHEN unique_violation THEN
            -- do nothing, and loop to try the UPDATE again
        END;
    END LOOP;
END
$$
LANGUAGE plpgsql;


CREATE TABLE car (
    mid int PRIMARY KEY,
    carstatu smallint NOT NULL DEFAULT 0, --0 fresh, 1 pause
    size smallint NOT NULL DEFAULT 0, --0 小轿车, 1 MPV, 2 SUV, 3 面包车, 4 大客车, 5 货车
    model varchar(64) NOT NULL DEFAULT '',
    mdate date DEFAULT '2011-10-10'
);
CREATE INDEX car_index ON car (size);

--keep plan clean (no history useless plan) please
CREATE TABLE plan (
    id SERIAL,
    mid int NOT NULL DEFAULT 0,
    phone varchar(128) NOT NULL DEFAULT '',
    contact varchar(128) NOT NULL DEFAULT '',

    ori smallint NOT NULL DEFAULT 0, -- plan's origin domain, see server.hdf
    oid varchar(64) NOT NULL DEFAULT '',   -- plan's origin id
    ourl varchar(256) NOT NULL DEFAULT '', -- plan's origin url

    dad smallint NOT NULL DEFAULT 0,   --0 person, 1 car
    nick varchar(32) NOT NULL DEFAULT '',
    statu smallint NOT NULL DEFAULT 0, --0 fresh, 1 pause, 2 delete, 10 spd fresh
    repeat smallint NOT NULL DEFAULT 0, --0 no, 1 day, 2 week
    subscribe smallint NOT NULL DEFAULT 0, -- same as addrtype of table expect
    seat smallint NOT NULL DEFAULT 4,
    fee int NOT NULL DEFAULT 0,
    attach varchar(256) NOT NULL DEFAULT '',

    cityid int NOT NULL DEFAULT 0,
    scityid int NOT NULL DEFAULT 0,
    ecityid int NOT NULL DEFAULT 0,
    saddr varchar(256) NOT NULL DEFAULT '',
    eaddr varchar(256) NOT NULL DEFAULT '',
    sgeo point NOT NULL DEFAULT '(0,0)',
    egeo point NOT NULL DEFAULT '(0,0)',
    marks varchar(256) NOT NULL DEFAULT '',

    rect box NOT NULL DEFAULT '((0,0), (1,1))',
    route path NOT NULL DEFAULT '[(0,0), (1,1)]',
    km numeric NOT NULL DEFAULT 0.1,

    sdate varchar(16) NOT NULL DEFAULT '2011-11-11', --or 1,3,5
    edate varchar(16) NOT NULL DEFAULT '2011-11-11',
    stime time NOT NULL DEFAULT '08:00:00',
    etime time NOT NULL DEFAULT '09:00:00',
    estmin int NOT NULL DEFAULT 0,
    
    intime timestamp DEFAULT now(),
    uptime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);
CREATE TRIGGER tg_uptime_plan BEFORE UPDATE ON plan FOR EACH ROW EXECUTE PROCEDURE update_time();
