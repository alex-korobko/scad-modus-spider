! dropping table channels
drop table channels;
;
! creating table channels
create TABLE CHANNELS(
CHANNEL_ID INTEGER PRIMARY KEY,
CHANNEL_LEFT_SIDE INTEGER,
CHANNEL_RIGHT_SIDE INTEGER,
CHANNEL_NUMBER INTEGER,
IS_BUSY INTEGER )
;
!IN-MEMORY AUTOSAVE AUTOLOAD; 
;
!dropping table channelb_trunk
drop table channelb_trunk;
;
!creating table channelb_trunk
create table channelb_trunk(
CHANNELB_ID INTEGER UNIQUE NOT NULL, 
TRUNK_ID INTEGER);
;
!dropping table channel_b_d
drop table channel_b_d;
;
!creating table channel_b_d
create table channel_b_d(
CHANNELB_ID INTEGER UNIQUE NOT NULL, 
CHANNELD_ID INTEGER NOT NULL,
CHANNELB_NUMBER INTEGER NOT NULL)
;
!IN-MEMORY AUTOSAVE AUTOLOAD;
;
!dropping table channel_d
drop table channel_d;
;
!creating table channel_d
create table channel_d(
CHANNELD_ID INTEGER PRIMARY KEY,
CHANNELB_ID INTEGER UNIQUE NOT NULL
);
!IN-MEMORY AUTOSAVE AUTOLOAD;
;
!filling table channels
execute filler_of_channels (4,16,16,32);
;
!filling table channelb_trunk
execute filler_of_chb_trunk ();
;
!filling table channel_d
execute filler_of_channel_d ();
;
!filling table channel_b_d
execute filler_of_channel_b_d ();
;
! creating index by fields channel_right_side, channel_left_side
! on  table channels
create index channels_sides on channels
(channel_right_side, channel_left_side);
;
! creating index by fields channeld_id, channelb_id
! on  table channels
create index channel_b_d_ids on channel_b_d
(channeld_id, channelb_id);
;

