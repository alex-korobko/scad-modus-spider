create or replace procedure "FILLER_OF_CHB_TRUNK"
            (in trunks_count int default 8) result int for debug
declare
  var trunk_i int;
  var channels_id_i int default 0;
  var channelb_in_trunk int;
  var channels_per_trunk int;
  var channb_ids cursor (id int); 
  var channb_ids_count cursor (count_value int);
  exception bad_cursor for badcursor;
  exception bad_parameter for badparam;
code

  execute direct "delete from channelb_trunk;";
  open channb_ids_count for direct "select count(*) from channels where channel_right_side-channel_left_side=1;" ;
  channels_per_trunk:=channb_ids_count.count_value/trunks_count;
  close channb_ids_count;
 
  open channb_ids for direct "select channel_id from channels where channel_right_side-channel_left_side=1 order by channel_id;" ;

  trunk_i:=1;
  channels_id_i:=1;
  channelb_in_trunk:=0;
    while not outofcursor(channb_ids) loop
      execute direct "insert into channelb_trunk (channelb_id, trunk_id) values ("+
                     itoa(channb_ids.id)+", "+itoa(trunk_i)+");";
      fetch channb_ids;  
      channels_id_i:=channels_id_i+1;
      channelb_in_trunk:=channelb_in_trunk+1;
      if (channelb_in_trunk>=channels_per_trunk) then
        channelb_in_trunk:=0;
	trunk_i:=trunk_i+1;
      endif;
   endloop

  return channels_id_i;
 
exceptions
  when others then
    print ("EXCEPTION : filler_of_channel_b_d(...) : unknown");
    resignal;
end;
