#ifndef __METRO_DEVICES_TYPES_CONT_TEST__
#define __METRO_DEVICES_TYPES_CONT_TEST__

class devices_types_container_test : public CppUnit::TestFixture {
private:
 string file_name;

void erase_signal_id_from_block(int id_signal_to_find, 
													devices_types_container::iterator_devices_types tmp_iter)
{
metro_device_type::iterator_blocks tmp_iter_blocks;
metro_device_type::iterator_signals tmp_iter_signals;
device_block::iterator_signals_id tmp_iter_signals_id;

tmp_iter_signals=tmp_iter->second.signals_find(id_signal_to_find);
CPPUNIT_ASSERT(tmp_iter_signals!=tmp_iter->second.signals_end());

tmp_iter_blocks=tmp_iter->second.blocks_begin();
while(tmp_iter_blocks!=tmp_iter->second.blocks_end())
	{
		if (!tmp_iter_blocks->second.signals_id_empty())
		{
		tmp_iter_signals_id=tmp_iter_blocks->second.signals_id_begin();
		while(tmp_iter_signals_id!=tmp_iter_blocks->second.signals_id_end())
			{
				if (*tmp_iter_signals_id==id_signal_to_find)
					{
						device_block::size_type_signals_id old_signals_id_size=tmp_iter_blocks->second.signals_id_size();
						tmp_iter_blocks->second.signals_id_erase(tmp_iter_signals_id);
						CPPUNIT_ASSERT(tmp_iter_blocks->second.signals_id_size()==(old_signals_id_size-1));		
						return ;
					}; 	 
				tmp_iter_signals_id++;
			};
		};

		tmp_iter_blocks++;
	};
	CPPUNIT_ASSERT(false); //id_signal_to_find in all blocks of device type not found
};
 
void test_insert()
{
g_metro_devices_types.insert(g_metro_devices_types.end(), devices_types_container::pair_devices_types(3, 
																										metro_device_type(3,"third type of devices")
																										)
								);




g_metro_devices_types.insert(g_metro_devices_types.begin(), devices_types_container::pair_devices_types(1, 
																										metro_device_type(1,"first type of devices")
																										)
								);

g_metro_devices_types.insert(g_metro_devices_types.upper_bound(2), devices_types_container::pair_devices_types(2, 
																										metro_device_type(2,"second type of devices")
																										)
							 );

devices_types_container::iterator_devices_types tmp_iter;
tmp_iter=g_metro_devices_types.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_metro_devices_types.end());

tmp_iter->second.blocks_insert(tmp_iter->second.blocks_end(),
									metro_device_type::pair_blocks (
											3,
											device_block(  3,
											"block 3 of device type 1"	)								
											)
									);

tmp_iter->second.blocks_insert(tmp_iter->second.blocks_begin(),
									metro_device_type::pair_blocks (
											1,
											device_block(  1,
											"block 1 of device type 1"	)								
											)
									);

tmp_iter->second.blocks_insert(tmp_iter->second.blocks_upper_bound(2),
									metro_device_type::pair_blocks (
											2,
											device_block(  2,
											"block 2 of device type 1"	)								
											)
									);

CPPUNIT_ASSERT( tmp_iter->second.blocks_size()==3);

metro_device_type::iterator_blocks tmp_iter_blocks=tmp_iter->second.blocks_find(1);
CPPUNIT_ASSERT( tmp_iter_blocks!=tmp_iter->second.blocks_end());

tmp_iter->second.signals_insert(tmp_iter->second.signals_end(),
									metro_device_type::pair_signals 
											(3,
											device_signal(  3,
																		"sig3",
																		"signal 3 of block 1 of device type 1"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(3);

tmp_iter->second.signals_insert(tmp_iter->second.signals_begin(),
									metro_device_type::pair_signals 
											(1,
											device_signal(  1,
																		"sig1",
																		"signal 1 of block 1 of device type 1"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(1);

tmp_iter->second.signals_insert(tmp_iter->second.signals_upper_bound(2),
									metro_device_type::pair_signals 
											(2,
											device_signal(  2,
																		"sig2",
																		"signal 2 of block 1 of device type 1"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(2);

tmp_iter_blocks=tmp_iter->second.blocks_find(2);
CPPUNIT_ASSERT( tmp_iter_blocks!=tmp_iter->second.blocks_end());

tmp_iter->second.signals_insert(tmp_iter->second.signals_upper_bound(4),
									metro_device_type::pair_signals 
											(4,
											device_signal(  4,
																		"sig4",
																		"signal 4 of block 2 of device type 1"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(4);

tmp_iter->second.signals_insert(tmp_iter->second.signals_upper_bound(5),
									metro_device_type::pair_signals 
											(5,
											device_signal(  5,
																		"sig5",
																		"signal 5 of block 2 of device type 1"
																	)
											)
									);

tmp_iter_blocks->second.signals_id_push_back(5);


msg_dict_container* msg_container=tmp_iter->second.get_messages();
msg_container->insert(msg_container->end(), msg_dict_container::msg_dict_pair(1, 
																										dictionary_message(1, 1,"message 1 of device type 1")
																										)
									);
									
msg_container->insert(msg_container->end(), msg_dict_container::msg_dict_pair(2, 
																										dictionary_message(2, 2,"message 2 of device type 1")
																										)
									);

msg_container->insert(msg_container->end(), msg_dict_container::msg_dict_pair(3, 
																										dictionary_message(3, 2,"message 3 of device type 1")
																										)
									);

tmp_iter=g_metro_devices_types.find(2);
CPPUNIT_ASSERT( tmp_iter!=g_metro_devices_types.end());

tmp_iter->second.blocks_insert(tmp_iter->second.blocks_end(),
									metro_device_type::pair_blocks (
											1,
											device_block(  1,
											"block 1 of device type 2"	)								
											)
									);

tmp_iter_blocks=tmp_iter->second.blocks_find(1);
CPPUNIT_ASSERT( tmp_iter_blocks!=tmp_iter->second.blocks_end());

tmp_iter->second.signals_insert(tmp_iter->second.signals_end(),
									metro_device_type::pair_signals 
											(1,
											device_signal(  1,
																		"sig1",
																		"signal 1 of block 1 of device type 2"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(1);


};


void test_erase()
{
devices_types_container::iterator_devices_types tmp_iter;
devices_types_container::size_devices_types old_size=
							g_metro_devices_types.size();

metro_device_type::iterator_blocks tmp_iter_blocks;
metro_device_type::iterator_signals tmp_iter_signals;

CPPUNIT_ASSERT(!g_metro_devices_types.empty());
CPPUNIT_ASSERT(old_size==3);

tmp_iter=g_metro_devices_types.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_metro_devices_types.end());

CPPUNIT_ASSERT( tmp_iter->second.blocks_size()==3);
CPPUNIT_ASSERT( !tmp_iter->second.blocks_empty());
CPPUNIT_ASSERT( tmp_iter->second.signals_size()==5);
CPPUNIT_ASSERT( !tmp_iter->second.signals_empty());

tmp_iter_signals=tmp_iter->second.signals_begin();
while(tmp_iter_signals!=tmp_iter->second.signals_end())
	{
	erase_signal_id_from_block(tmp_iter_signals->second.get_index(), 
											tmp_iter);
	tmp_iter_signals++;
	};										

tmp_iter_blocks=tmp_iter->second.blocks_begin();
while (! tmp_iter->second.blocks_empty() &&
			 tmp_iter_blocks!=tmp_iter->second.blocks_end())
	{
	tmp_iter->second.blocks_erase(tmp_iter_blocks++);
	};										

g_metro_devices_types.erase(tmp_iter);
CPPUNIT_ASSERT(!g_metro_devices_types.empty());
CPPUNIT_ASSERT(old_size==g_metro_devices_types.size()+1);


g_metro_devices_types.erase(g_metro_devices_types.begin(),
												g_metro_devices_types.end());

CPPUNIT_ASSERT(g_metro_devices_types.empty());
CPPUNIT_ASSERT(g_metro_devices_types.size()==0);
};

void test_load()
{
//ATTENTION  - all of that messages types must be in file_name !!!
/*
#example of right file_name file
[type]
id=1
name=first type of devices

	[block]
	id=1
	name=block 1 of device type 1

			[signal]
			index=1
			name=sig1
			hint=signal 1 of block 1 of device type 1

			[signal]
			index=3
			name=sig3
			hint=signal 3 of block 1 of device type 1

	[block]
	id=2
	name=block 2 of device type 1

			[signal]
			index=2
			block=1
			name=sig2
			hint=signal 2 of block 1 of device type 1

			[signal]
			index=4
			name=sig4
			hint=signal 4 of block 2 of device type 1


	[message]
	id=1
	message type=1
	text=message 1 of device type 1

	[message]
	id=2
	message type=2
	text=message 2 of device type 1

[type]
id=2
name=second type of devices

	[block]
	id=1
	name=block 1 of device type 2

			[signal]
			index=5
			block=2
			type=1
			name=sig5
			hint=signal 5 of block 2 of device type 1

			[signal]
			index=1
			name=sig1
			hint=signal 1 of block 1 of device type 2

	[block]
	id=3
	type=1
	name=block 3 of device type 1
	
	[message]
	id=3
	message type=2
	type=1
	text=message 3 of device type 1


[type]
id=3
name=third type of devices
*/
devices_types_container::iterator_devices_types tmp_iter1, tmp_iter2;

test_insert();
devices_types_container tmp_devices_types_container;
tmp_devices_types_container.load(&g_system_settings, &g_msg_types, file_name);

CPPUNIT_ASSERT(!tmp_devices_types_container.empty());
CPPUNIT_ASSERT(!g_metro_devices_types.empty());
CPPUNIT_ASSERT(g_metro_devices_types.size()==tmp_devices_types_container.size());

tmp_iter1=tmp_devices_types_container.begin();
while (tmp_iter1!=tmp_devices_types_container.end())
	{
		tmp_iter2=g_metro_devices_types.find(tmp_iter1->second.get_id());
		CPPUNIT_ASSERT(tmp_iter2!=g_metro_devices_types.end());		

		CPPUNIT_ASSERT(tmp_iter1->second.get_id()==tmp_iter2->second.get_id());		
		CPPUNIT_ASSERT(tmp_iter1->second.get_name().compare(tmp_iter2->second.get_name()) == 0);		
		CPPUNIT_ASSERT(tmp_iter1->second.blocks_size()==tmp_iter2->second.blocks_size());		
		CPPUNIT_ASSERT(tmp_iter1->second.signals_size()==tmp_iter2->second.signals_size());		

		metro_device_type::iterator_blocks tmp_iter_blocks1, tmp_iter_blocks2;
		tmp_iter_blocks1=tmp_iter1->second.blocks_begin();
		while (tmp_iter_blocks1!=tmp_iter1->second.blocks_end())
			{
				tmp_iter_blocks2=tmp_iter2->second.blocks_find(tmp_iter_blocks1->second.get_id());
				CPPUNIT_ASSERT(tmp_iter_blocks2!=tmp_iter2->second.blocks_end());
				CPPUNIT_ASSERT(tmp_iter_blocks2->second.get_id()==tmp_iter_blocks1->second.get_id());
				CPPUNIT_ASSERT(tmp_iter_blocks2->second.get_name().compare(tmp_iter_blocks1->second.get_name())==0);
				CPPUNIT_ASSERT(tmp_iter_blocks2->second.signals_id_size ()== tmp_iter_blocks1->second.signals_id_size ());

				device_block::iterator_signals_id tmp_iter_signals_id1=tmp_iter_blocks1->second.signals_id_begin(), 
																		tmp_iter_signals_id2;
				while(tmp_iter_signals_id1!=tmp_iter_blocks1->second.signals_id_end())
				{
				tmp_iter_signals_id2=find (tmp_iter_blocks2->second.signals_id_begin(),
														tmp_iter_blocks2->second.signals_id_end(),
														*tmp_iter_signals_id1);
				CPPUNIT_ASSERT(tmp_iter_signals_id2!=tmp_iter_blocks2->second.signals_id_end());

				tmp_iter_signals_id1++;
				};

			tmp_iter_blocks1++;
			}

		metro_device_type::iterator_signals tmp_iter_signals1=tmp_iter1->second.signals_begin(),
																	tmp_iter_signals2;

		while (tmp_iter_signals1!=tmp_iter1->second.signals_end())
		{
			tmp_iter_signals2=tmp_iter2->second.signals_find(tmp_iter_signals1->second.get_index());
		    CPPUNIT_ASSERT(tmp_iter_signals2!=tmp_iter2->second.signals_end());

		    CPPUNIT_ASSERT(tmp_iter_signals1->second.get_index()==tmp_iter_signals2->second.get_index());
		    CPPUNIT_ASSERT(tmp_iter_signals1->second.get_name().compare(tmp_iter_signals2->second.get_name())==0 );
		    CPPUNIT_ASSERT(tmp_iter_signals1->second.get_hint().compare(tmp_iter_signals2->second.get_hint())==0);
		    
			tmp_iter_signals1++;
		};

		tmp_iter1++;
	};

};

 
public:
void setUp()
{
	file_name="../unit_tests/devices_types_test.ini";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Tests for Container of Metro devices Types ");

suite_of_tests->addTest(new CppUnit::TestCaller<devices_types_container_test> 
												( "insert metod test",
													&devices_types_container_test::test_insert)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<devices_types_container_test> 
												( "erase metod test",
													&devices_types_container_test::test_erase)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<devices_types_container_test> 
												( "load metod test",
													&devices_types_container_test::test_load)
										);


return suite_of_tests;
}
};

#endif