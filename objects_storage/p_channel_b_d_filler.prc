create or replace procedure "FILLER_OF_CHANNEL_B_D"
            () result int for debug
declare
  var channeld_i int;
  var channd_ids cursor (id_d int, id_b int, num_b int);
  var inserts_count int;
  exception bad_cursor for badcursor;
code

  execute direct "delete from channel_b_d;";
 
  open channd_ids for direct "select channel_d.channeld_id, channel_d.channelb_id, channels.channel_number from channel_d, channels where  channel_d.channelb_id=channels.channel_id order by channel_d.channeld_id;";
  
  inserts_count:=1;
  while not outofcursor(channd_ids) loop

        execute direct "insert into channel_b_d (channeld_id, channelb_id, channelb_number) values ("+
                     itoa(channd_ids.id_d)+", "+itoa(channd_ids.id_b)+", 1);"; // channel b number always 1 for voice

	inserts_count:=inserts_count+1;
	fetch channd_ids;
  endloop
 
  return inserts_count;
 
exceptions
  when others then
    print ("EXCEPTION : FILLER_OF_CHANNEL_B_D(...) : unknown"); //
    resignal; //
end;
