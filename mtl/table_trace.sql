-- event big map
CREATE TABLE emap (
	id SERIAL,
    type smallint NOT NULL DEFAULT 0, -- 0 pv, 1 regist, 2 comment...
    sender varchar(256) NOT NULL DEFAULT '', -- username, membername
    provid int NOT NULL DEFAULT 0,           -- province id
    cityid int NOT NULL DEFAULT 0,           -- smalleset city id
    browser smallint NOT NULL DEFAULT 0, -- 0 firefox, 1 chorme, 2 ie...
    bversion real NOT NULL DEFAULT 0.0,

    ei_one   int NOT NULL DEFAULT 0,
    ei_two   int NOT NULL DEFAULT 0,
    ei_three int NOT NULL DEFAULT 0,
    es_one   varchar(256) NOT NULL DEFAULT '',
    es_two   varchar(256) NOT NULL DEFAULT '',
    es_three varchar(256) NOT NULL DEFAULT '',
    et_one   text NOT NULL DEFAULT '',
    et_two   text NOT NULL DEFAULT '',
    et_three text NOT NULL DEFAULT '',

    intime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);
CREATE INDEX emap_index ON emap (type, cityid, browser);
