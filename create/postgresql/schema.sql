--
-- Table structure for table 'hosts'
--

\connect allview

CREATE TABLE hosts (
  hostid		serial,
  host			varchar(64)	DEFAULT '' 		NOT NULL,
  useip			int4		DEFAULT '0'		NOT NULL,
  ip			varchar(15)	DEFAULT '127.0.0.1'	NOT NULL,
  port			int4		DEFAULT '0'		NOT NULL,
  status		int4		DEFAULT '0'		NOT NULL,
  disable_until		int4		DEFAULT '0'		NOT NULL,
  network_errors	int4		DEFAULT '0'		NOT NULL,
  error			varchar(128)	DEFAULT ''		NOT NULL,
  available		int4		DEFAULT '0'		NOT NULL,
  PRIMARY KEY (hostid)
);

CREATE INDEX hosts_status on hosts (status);
CREATE UNIQUE INDEX hosts_host on hosts (host);

--
-- Table structure for table 'items'
--

CREATE TABLE items (
  itemid		serial,
  type			int4		NOT NULL,
  snmp_community	varchar(64)	DEFAULT ''	NOT NULL,
  snmp_oid		varchar(255)	DEFAULT ''	NOT NULL,
  snmp_port		int4		DEFAULT '161'	NOT NULL,
  hostid		int4		NOT NULL,
  description		varchar(255)	DEFAULT '' NOT NULL,
  key_			varchar(64)	DEFAULT '' NOT NULL,
  delay			int4		DEFAULT '0' NOT NULL,
  history		int4		DEFAULT '90' NOT NULL,
  trends		int4		DEFAULT '365' NOT NULL,
-- lastdelete is no longer required
--  lastdelete		int4		DEFAULT '0' NOT NULL,
  nextcheck		int4		DEFAULT '0' NOT NULL,
  lastvalue		varchar(255)	DEFAULT NULL,
  lastclock		int4		DEFAULT NULL,
  prevvalue		varchar(255)	DEFAULT NULL,
  status		int4		DEFAULT '0' NOT NULL,
  value_type		int4		DEFAULT '0' NOT NULL,
  trapper_hosts		varchar(255)	DEFAULT '' NOT NULL,
  units			varchar(10)	DEFAULT '' NOT NULL,
  multiplier		int4		DEFAULT '0' NOT NULL,
  delta			int4		DEFAULT '0' NOT NULL,
  prevorgvalue		float8		DEFAULT NULL,
  snmpv3_securityname	varchar(64)	DEFAULT '' NOT NULL,
  snmpv3_securitylevel	int4		DEFAULT '0' NOT NULL,
  snmpv3_authpassphrase	varchar(64)	DEFAULT '' NOT NULL,
  snmpv3_privpassphrase	varchar(64)	DEFAULT '' NOT NULL,
  formula		varchar(255)	DEFAULT '{.last(0)}' NOT NULL,
  error			varchar(128)	DEFAULT '' NOT NULL,
  PRIMARY KEY (itemid),
  FOREIGN KEY (hostid) REFERENCES hosts
);

CREATE UNIQUE INDEX items_hostid_key on items (hostid,key_);
CREATE INDEX items_hostid on items (hostid);
CREATE INDEX items_nextcheck on items (nextcheck);
CREATE INDEX items_status on items (status);

--
-- Table structure for table 'config'
--

CREATE TABLE config (
--  smtp_server		varchar(255)	DEFAULT '' NOT NULL,
--  smtp_helo		varchar(255)	DEFAULT '' NOT NULL,
--  smtp_email		varchar(255)	DEFAULT '' NOT NULL,
--  password_required	int4		DEFAULT '0' NOT NULL,
  alert_history		int4		DEFAULT '0' NOT NULL,
  alarm_history		int4		DEFAULT '0' NOT NULL
);

--
-- Table structure for table 'groups'
--

CREATE TABLE groups (
  groupid		serial,
  name			varchar(64)     DEFAULT '' NOT NULL,
  PRIMARY KEY (groupid),
  UNIQUE (name)
);

CREATE UNIQUE INDEX groups_name on groups (name);

--
-- Table structure for table 'hosts_groups'
--

CREATE TABLE hosts_groups (
  hostid		int4		DEFAULT '0' NOT NULL,
  groupid		int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY (hostid,groupid)
);

--CREATE UNIQUE INDEX hosts_groups_name on hosts_groups (hostid,groupid);

--
-- Table structure for table 'triggers'
--

CREATE TABLE triggers (
  triggerid		serial,
  expression		varchar(255)	DEFAULT '' NOT NULL,
  description		varchar(255)	DEFAULT '' NOT NULL,
  url			varchar(255)	DEFAULT '' NOT NULL,
  status		int4		DEFAULT '0' NOT NULL,
  value			int4		DEFAULT '0' NOT NULL,
  priority		int2		DEFAULT '0' NOT NULL,
  lastchange		int4		DEFAULT '0' NOT NULL,
  dep_level		int2		DEFAULT '0' NOT NULL,
  comments		text,
  PRIMARY KEY (triggerid)
);

CREATE INDEX triggers_value on triggers (value);
CREATE INDEX triggers_status on triggers (status);

--
-- Table structure for table 'trigger_depends'
--

CREATE TABLE trigger_depends (
  triggerid_down	int4	DEFAULT '0' NOT NULL,
  triggerid_up		int4	DEFAULT '0' NOT NULL,
  PRIMARY KEY		(triggerid_down, triggerid_up)
);

CREATE INDEX trigger_depends_down on trigger_depends (triggerid_down);
CREATE INDEX trigger_depends_up   on trigger_depends (triggerid_up);

--
-- Table structure for table 'users'
--

CREATE TABLE users (
  userid		serial,
  alias			varchar(100)	DEFAULT '' NOT NULL,
  name			varchar(100)	DEFAULT '' NOT NULL,
  surname		varchar(100)	DEFAULT '' NOT NULL,
  passwd		char(32)	DEFAULT '' NOT NULL,
  url			varchar(255)	DEFAULT '' NOT NULL,
  PRIMARY KEY (userid)
);

CREATE UNIQUE INDEX users_alias on users (alias);

--
-- Table structure for table 'audit'
--

CREATE TABLE audit (
  auditid		serial,
  userid		int4		DEFAULT '0' NOT NULL,
  clock			int4		DEFAULT '0' NOT NULL,
  action		int4		DEFAULT '0' NOT NULL,
  resource		int4		DEFAULT '0' NOT NULL,
  details		varchar(128)	DEFAULT '0' NOT NULL,
  PRIMARY KEY (auditid)
);

CREATE UNIQUE INDEX audit_userid_clock on audit (userid,clock);
CREATE INDEX audit_clock on audit (clock);

--
-- Table structure for table 'actions'
--

CREATE TABLE actions (
  actionid		serial,
  triggerid		int4		DEFAULT '0' NOT NULL,
  userid		int4		DEFAULT '0' NOT NULL,
  scope			int4		DEFAULT '0' NOT NULL,
  severity		int4		DEFAULT '0' NOT NULL,
  good			int4		DEFAULT '0' NOT NULL,
  delay			int4		DEFAULT '0' NOT NULL,
  subject		varchar(255)	DEFAULT '' NOT NULL,
  message		text		DEFAULT '' NOT NULL,
  nextcheck		int4		DEFAULT '0' NOT NULL,
  recipient		int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY (actionid)
--  depends on scope. Could be hostid or 0.
--  FOREIGN KEY (triggerid) REFERENCES triggers
--  could be groupid
--  FOREIGN KEY (userid) REFERENCES users
);

--
-- Table structure for table 'media_type'
--

CREATE TABLE media_type (
  mediatypeid		serial,
  type			int4		DEFAULT '0' NOT NULL,
  description		varchar(100)	DEFAULT '' NOT NULL,
  smtp_server		varchar(255)	DEFAULT '' NOT NULL,
  smtp_helo		varchar(255)	DEFAULT '' NOT NULL,
  smtp_email		varchar(255)	DEFAULT '' NOT NULL,
  exec_path		varchar(255)	DEFAULT '' NOT NULL,
  PRIMARY KEY(mediatypeid)
);


--
-- Table structure for table 'alerts'
--

CREATE TABLE alerts (
  alertid		serial,
  actionid		int4		DEFAULT '0' NOT NULL,
  clock			int4		DEFAULT '0' NOT NULL,
--  type		varchar(10)	DEFAULT '' NOT NULL,
  mediatypeid		int4		DEFAULT '0' NOT NULL,
  sendto		varchar(100)	DEFAULT '' NOT NULL,
  subject		varchar(255)	DEFAULT '' NOT NULL,
  message		text		DEFAULT '' NOT NULL,
  status		int4		DEFAULT '0' NOT NULL,
  retries		int4		DEFAULT '0' NOT NULL,
  error			varchar(128)	DEFAULT '' NOT NULL,
  PRIMARY KEY (alertid),
  FOREIGN KEY (actionid) REFERENCES actions,
  FOREIGN KEY (mediatypeid) REFERENCES media_type
);

CREATE INDEX alerts_actionid on alerts (actionid);
CREATE INDEX alerts_clock on alerts (clock);
CREATE INDEX alerts_status_retires on alerts (status,retries);

--
-- Table structure for table 'alarms'
--

CREATE TABLE alarms (
  alarmid		serial,
  triggerid		int4		DEFAULT '0' NOT NULL,
  clock			int4		DEFAULT '0' NOT NULL,
  istrue		int4		DEFAULT '0' NOT NULL,
  value			int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY (alarmid),
  FOREIGN KEY (triggerid) REFERENCES triggers
);

CREATE INDEX alarms_triggerid_clock on alarms (triggerid, clock);
CREATE INDEX alarms_clock on alarms (clock);

--
-- Table structure for table 'functions'
--

CREATE TABLE functions (
  functionid		serial,
  itemid		int4		DEFAULT '0' NOT NULL,
  triggerid		int4		DEFAULT '0' NOT NULL,
  lastvalue		varchar(255),
  function		varchar(10)	DEFAULT '' NOT NULL,
  parameter		varchar(255)	DEFAULT '0' NOT NULL,
  PRIMARY KEY (functionid),
  FOREIGN KEY (itemid) REFERENCES items,
  FOREIGN KEY (triggerid) REFERENCES triggers
);

CREATE INDEX funtions_triggerid on functions (triggerid);
CREATE INDEX functions_i_f_p on functions (itemid,function,parameter);

--
-- Table structure for table 'history'
--

CREATE TABLE history (
  itemid		int4		DEFAULT '0' NOT NULL,
  clock			int4		DEFAULT '0' NOT NULL,
  value			float8		DEFAULT '0.0000' NOT NULL,
--  PRIMARY KEY (itemid,clock),
  FOREIGN KEY (itemid) REFERENCES items
);

CREATE INDEX history_i_c on history (itemid, clock);

--
-- Table structure for table 'history_str'
--

CREATE TABLE history_str (
  itemid                int4            DEFAULT '0' NOT NULL,
  clock                 int4            DEFAULT '0' NOT NULL,
  value                 varchar(255)    DEFAULT '' NOT NULL,
--  PRIMARY KEY (itemid,clock),
  FOREIGN KEY (itemid) REFERENCES items
);

CREATE INDEX history_str_i_c on history_str (itemid, clock);

--
-- Table structure for table 'items_template'
--

CREATE TABLE items_template (
  itemtemplateid	int4		NOT NULL,
  description		varchar(255)	DEFAULT '' NOT NULL,
  key_			varchar(64)	DEFAULT '' NOT NULL,
  delay			int4		DEFAULT '0' NOT NULL,
  value_type		int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY (itemtemplateid)
);

CREATE UNIQUE INDEX items_template_p_k on items_template (key_);

--
-- Table structure for table 'triggers_template'
--

CREATE TABLE triggers_template (
  triggertemplateid	int4		NOT NULL,
  itemtemplateid	int4		NOT NULL,
  description		varchar(255)	DEFAULT '' NOT NULL,
  expression		varchar(255)	DEFAULT '' NOT NULL,
  PRIMARY KEY (triggertemplateid),
  FOREIGN KEY (itemtemplateid) REFERENCES items_template
);

--
-- Table structure for table 'media'
--

CREATE TABLE media (
  mediaid		serial,
  userid		int4		DEFAULT '0' NOT NULL,
--  type		varchar(10)	DEFAULT '' NOT NULL,
  mediatypeid		int4		DEFAULT '0' NOT NULL,
  sendto		varchar(100)	DEFAULT '' NOT NULL,
  active		int4		DEFAULT '0' NOT NULL,
  severity		int4		DEFAULT '63' NOT NULL,
  PRIMARY KEY (mediaid),
  FOREIGN KEY (userid) REFERENCES users,
  FOREIGN KEY (mediatypeid) REFERENCES media_type
);

--
-- Table structure for table 'sysmaps'
--

CREATE TABLE sysmaps (
  sysmapid		serial,
  name			varchar(128)	DEFAULT '' NOT NULL,
  width			int4		DEFAULT '0' NOT NULL,
  height		int4		DEFAULT '0' NOT NULL,
  background		varchar(64)	DEFAULT '' NOT NULL,
  label_type		int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY (sysmapid)
);

CREATE UNIQUE INDEX sysmaps_name on sysmaps (name);

--
-- Table structure for table 'sysmaps_hosts'
--

CREATE TABLE sysmaps_hosts (
  shostid		serial,
  sysmapid		int4		DEFAULT '0' NOT NULL,
  hostid		int4		DEFAULT '0' NOT NULL,
  icon			varchar(32)	DEFAULT 'Server' NOT NULL,
  icon_on		varchar(32)	DEFAULT 'Server' NOT NULL,
  label			varchar(128)	DEFAULT '' NOT NULL,
  x			int4		DEFAULT '0' NOT NULL,
  y			int4		DEFAULT '0' NOT NULL,
  url			varchar(255)	DEFAULT '' NOT NULL,
  PRIMARY KEY (shostid),
  FOREIGN KEY (sysmapid) REFERENCES sysmaps,
  FOREIGN KEY (hostid) REFERENCES hosts
);

--
-- Table structure for table 'sysmaps_links'
--

CREATE TABLE sysmaps_links (
  linkid		serial,
  sysmapid		int4		DEFAULT '0' NOT NULL,
  shostid1		int4		DEFAULT '0' NOT NULL,
  shostid2		int4		DEFAULT '0' NOT NULL,
-- may be NULL 
  triggerid		int4,
  drawtype_off		int4		DEFAULT '0' NOT NULL,
  color_off		varchar(32)	DEFAULT 'Black' NOT NULL,
  drawtype_on		int4		DEFAULT '0' NOT NULL,
  color_on		varchar(32)	DEFAULT 'Red' NOT NULL,
  PRIMARY KEY (linkid),
  FOREIGN KEY (sysmapid) REFERENCES sysmaps,
  FOREIGN KEY (shostid1) REFERENCES sysmaps_hosts,
  FOREIGN KEY (shostid2) REFERENCES sysmaps_hosts
);

--
-- Table structure for table 'graphs'
--

CREATE TABLE graphs (
  graphid		serial,
  name			varchar(128)	DEFAULT '' NOT NULL,
  width			int4		DEFAULT '0' NOT NULL,
  height		int4		DEFAULT '0' NOT NULL,
  yaxistype		int2		DEFAULT '0' NOT NULL,
  yaxismin		float8		DEFAULT '0' NOT NULL,
  yaxismax		float8		DEFAULT '0' NOT NULL,
  PRIMARY KEY (graphid),
  UNIQUE (name)
);

CREATE UNIQUE INDEX graphs_name on graphs (name);

--
-- Table structure for table 'graphs_items'
--

CREATE TABLE graphs_items (
  gitemid		serial,
  graphid		int4		DEFAULT '0' NOT NULL,
  itemid		int4		DEFAULT '0' NOT NULL,
  drawtype		int4		DEFAULT '0' NOT NULL,
  sortorder		int4		DEFAULT '0' NOT NULL,
  color			varchar(32)	DEFAULT 'Dark Green' NOT NULL,
  PRIMARY KEY (gitemid),
  FOREIGN KEY (graphid) REFERENCES graphs,
  FOREIGN KEY (itemid) REFERENCES items
);

--
-- Table structure for table 'services'
--

CREATE TABLE services (
  serviceid		serial,
  name			varchar(128)	DEFAULT '' NOT NULL,
  status		int2		DEFAULT '0' NOT NULL,
  algorithm		int2		DEFAULT '0' NOT NULL,
  triggerid		int4,
  showsla		int4		DEFAULT '0' NOT NULL,
  goodsla		float8		DEFAULT '99.9' NOT NULL,
  sortorder		int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY (serviceid)
);

--
-- Table structure for table 'services_links'
--

CREATE TABLE services_links (
  linkid		serial,
  serviceupid		int4		DEFAULT '0' NOT NULL,
  servicedownid		int4		DEFAULT '0' NOT NULL,
  soft			int2		DEFAULT '0' NOT NULL,
  PRIMARY KEY (linkid)
);

CREATE INDEX services_links_serviceupid on services_links (serviceupid);
CREATE INDEX services_links_servicedownid on services_links (servicedownid);
CREATE UNIQUE INDEX services_links_upidownid on services_links (serviceupid, servicedownid);

CREATE TABLE rights (
  rightid               serial,
  userid                int4		DEFAULT '0' NOT NULL,
  name                  char(255)	DEFAULT '' NOT NULL,
  permission            char(1)		DEFAULT '' NOT NULL,
  id                    int4,
  PRIMARY KEY (rightid)
);

CREATE TABLE sessions (
	sessionid	varchar(32)	DEFAULT '' NOT NULL,
	userid		int4		DEFAULT '0' NOT NULL,
	lastaccess	int4		DEFAULT '0' NOT NULL,
	PRIMARY KEY (sessionid),
	FOREIGN KEY (userid) REFERENCES users
);

--
-- Table structure for table 'services_alarms'
--

CREATE TABLE service_alarms (
  servicealarmid	serial,
  serviceid		int4		DEFAULT '0' NOT NULL,
  clock			int4		DEFAULT '0' NOT NULL,
  value			int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY (servicealarmid)
);

CREATE INDEX services_alarms_serviceid_clock on service_alarms (serviceid,clock);
CREATE INDEX services_alarms_clock on service_alarms (clock);

--
-- Table structure for table 'profiles'
--

CREATE TABLE profiles (
  profileid		serial,
  userid		int4		DEFAULT '0' NOT NULL,
  idx			varchar(64)	DEFAULT '' NOT NULL,
  value			varchar(64)	DEFAULT '' NOT NULL,
  PRIMARY KEY (profileid)
);

CREATE INDEX profiles_userid on profiles (userid);
CREATE UNIQUE INDEX profiles_userid_idx on profiles (userid,idx);

--
-- Table structure for table 'screens'
--

CREATE TABLE screens (
  screenid		serial,
  name			varchar(255)	DEFAULT 'Screen' NOT NULL,
  cols			int4		DEFAULT '1' NOT NULL,
  rows			int4		DEFAULT '1' NOT NULL,
  PRIMARY KEY  (screenid)
);

--
-- Table structure for table 'screens_items'
--

CREATE TABLE screens_items (
  screenitemid		serial,
  screenid		int4		DEFAULT '0' NOT NULL,
  resource		int4		DEFAULT '0' NOT NULL,
  resourceid		int4		DEFAULT '0' NOT NULL,
  width			int4		DEFAULT '320' NOT NULL,
  height		int4		DEFAULT '200' NOT NULL,
  x			int4		DEFAULT '0' NOT NULL,
  y			int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY  (screenitemid)
);

--
-- Table structure for table 'stats'
--

CREATE TABLE stats (
  itemid		int4		DEFAULT '0' NOT NULL,
  year			int4		DEFAULT '0' NOT NULL,
  month			int4		DEFAULT '0' NOT NULL,
  day			int4		DEFAULT '0' NOT NULL,
  hour			int4		DEFAULT '0' NOT NULL,
  value_max		float8		DEFAULT '0.0000' NOT NULL,
  value_min		float8		DEFAULT '0.0000' NOT NULL,
  value_avg		float8		DEFAULT '0.0000' NOT NULL,
  PRIMARY KEY (itemid,year,month,day,hour)
);

--
-- Table structure for table 'usrgrp'
--

CREATE TABLE usrgrp (
  usrgrpid		serial,
  name			varchar(64)	DEFAULT '' NOT NULL,
  PRIMARY KEY (usrgrpid)
);

CREATE UNIQUE INDEX usrgrp_name on usrgrp (name);

--
-- Table structure for table 'users_groups'
--

CREATE TABLE users_groups (
  usrgrpid		int4		DEFAULT '0' NOT NULL,
  userid		int4		DEFAULT '0' NOT NULL,
  PRIMARY KEY (usrgrpid,userid),
  FOREIGN KEY (usrgrpid) REFERENCES usrgrp,
  FOREIGN KEY (userid) REFERENCES users
);

--
-- Table structure for table 'trends'
--

CREATE TABLE trends (
  itemid		int4		DEFAULT '0' NOT NULL,
  clock			int4		DEFAULT '0' NOT NULL,
  num			int2		DEFAULT '0' NOT NULL,
  value_min		float8		DEFAULT '0.0000' NOT NULL,
  value_avg		float8		DEFAULT '0.0000' NOT NULL,
  value_max		float8		DEFAULT '0.0000' NOT NULL,
  PRIMARY KEY (itemid,clock),
  FOREIGN KEY (itemid) REFERENCES items
);

--
-- Table structure for table 'escalations'
--

CREATE TABLE escalations (
  escalationid		serial,
  name			varchar(64)	DEFAULT '0' NOT NULL,
  PRIMARY KEY (escalationid)
);

CREATE UNIQUE INDEX escalations_name on escalations (name);

--
-- Table structure for table 'hosts_templates'
--

CREATE TABLE hosts_templates (
  hosttemplateid	serial,
  hostid		int4		DEFAULT '0' NOT NULL,
  templateid		int4		DEFAULT '0' NOT NULL,
  items			int2		DEFAULT '0' NOT NULL,
  triggers		int2		DEFAULT '0' NOT NULL,
  actions		int2		DEFAULT '0' NOT NULL,
  graphs		int2		DEFAULT '0' NOT NULL,
  screens		int2		DEFAULT '0' NOT NULL,
  PRIMARY KEY (hosttemplateid)
);

CREATE UNIQUE INDEX hosts_templates_hostid_templateid on hosts_templates (hostid, templateid);

VACUUM ANALYZE;
