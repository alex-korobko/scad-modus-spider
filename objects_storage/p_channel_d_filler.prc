create or replace procedure "FILLER_OF_CHANNEL_D"
            () result int for debug
declare
  var channeld_i int;
  var channb_ids cursor (id int);
  exception bad_cursor for badcursor;
  exception bad_parameter for badparam;
code

  execute direct "delete from channel_d;";
 
  open channb_ids for direct "select channel_id from channels where channel_right_side-channel_left_side=1 order by channel_id;";

  channeld_i:=1;
  while not outofcursor(channb_ids) loop      
      execute direct "insert into channel_d (channeld_id, channelb_id) values ("
                     +itoa(channeld_i)+", "+itoa(channb_ids.id)+");";
      channeld_i:=channeld_i+1;
      fetch channb_ids;
   endloop

  return channeld_i;
 
exceptions
  when others then
    print ("EXCEPTION : FILLER_OF_CHANNEL_D(...) : unknown");
    resignal;


end;
