create or replace procedure "FILLER_OF_CHANNELS" 
            (in upo_count int default 4;
	     in ie1_count int default 16;
	     in e1_count int default 16;
	     in channelb_count int default 32) result int for debug
declare
  var left_side_id int default 0;
  var upo_i int;
  var ie1_i int;
  var e1_i int;
  var channelb_i int;
  var channels_id_i int default 1;

  var upo_distance int;
  var ie1_distance int;
  var e1_distance int;
  var chb_distance int;

  exception bad_cursor for badcursor;
  exception bad_parameter for badparam;
code

 upo_distance:=2*ie1_count+2*ie1_count*e1_count+2*ie1_count*e1_count*channelb_count+1;
 ie1_distance:=2*e1_count+2*e1_count*channelb_count+1;
 e1_distance:=2*channelb_count+1;
 chb_distance:=1;

 execute direct "delete from channels;";

  upo_i := 0;
  while upo_i <upo_count loop
     left_side_id:=upo_i*upo_distance+upo_i+1;
     execute direct "insert into channels (channel_id, channel_left_side, channel_right_side, channel_number, is_busy  )  " +
     " values("+itoa(channels_id_i)+", "+     
       itoa(left_side_id)+", "+
       itoa(left_side_id+upo_distance)+
        ", "+itoa(upo_i)+",0);"; 
     channels_id_i:=channels_id_i+1;

          ie1_i := 0;
         while ie1_i < ie1_count loop
              left_side_id:=upo_i*upo_distance+ie1_i*ie1_distance+upo_i+ie1_i+2;
              execute direct "insert into channels (channel_id, channel_left_side, channel_right_side, channel_number, is_busy  )  " +
                     " values("+itoa(channels_id_i)+", "+
                   itoa(left_side_id)+", "+
                   itoa(left_side_id+ie1_distance)+", "+itoa(ie1_i)+",0);"; 
              channels_id_i:=channels_id_i+1;

             e1_i := 0;
             while e1_i < e1_count loop
                 left_side_id:=upo_i*upo_distance+ie1_i*ie1_distance+e1_i*e1_distance+upo_i+ie1_i+e1_i+3;
                 execute direct "insert into channels (channel_id, channel_left_side, channel_right_side, channel_number, is_busy  )  " +
                     " values("+itoa(channels_id_i)+", "+
                       itoa(left_side_id)+", "+
                       itoa(left_side_id+e1_distance)+", "+
                       itoa(e1_i)+",0);";
                 channels_id_i:=channels_id_i+1;

                channelb_i := 0;
                while channelb_i < channelb_count loop
                     left_side_id:=upo_i*upo_distance+ie1_i*ie1_distance+e1_i*e1_distance+channelb_i*chb_distance+upo_i+ie1_i+e1_i+channelb_i+4;
                     execute direct "insert into channels (channel_id, channel_left_side, channel_right_side, channel_number, is_busy  )  " +
                        " values("+itoa(channels_id_i)+", "+
                         itoa(left_side_id)+", "+
                         itoa(left_side_id+chb_distance)+", "+
                         itoa(channelb_i)+",0);";
                        channels_id_i:=channels_id_i+1;

                         channelb_i :=channelb_i +1;
                 endloop

                 e1_i :=e1_i +1;
             endloop


              ie1_i :=ie1_i +1;
          endloop

     
     upo_i :=upo_i +1;
  endloop

  return (channels_id_i-1);

exceptions
  when others then
    print ("EXCEPTION : filler_of_channels(...) : unknown");
    resignal;
end;
