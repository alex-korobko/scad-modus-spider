#ifndef __METRO_ESCALATORS_TYPES_CONT_TEST__
#define __METRO_ESCALATORS_TYPES_CONT_TEST__

class esc_types_container_test : public CppUnit::TestFixture {
private:
 string file_name;

void erase_signal_id_from_block(int id_signal_to_find, 
													esc_types_container::iterator_esc_types tmp_iter)
{
metro_escalator_type::iterator_blocks tmp_iter_blocks;
metro_escalator_type::iterator_signals tmp_iter_signals;
escalator_block::iterator_signals_id tmp_iter_signals_id;

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
						escalator_block::size_type_signals_id old_signals_id_size=tmp_iter_blocks->second.signals_id_size();
						tmp_iter_blocks->second.signals_id_erase(tmp_iter_signals_id);
						CPPUNIT_ASSERT(tmp_iter_blocks->second.signals_id_size()==(old_signals_id_size-1));		
						return ;
					}; 	 
				tmp_iter_signals_id++;
			};
		};

		tmp_iter_blocks++;
	};
	CPPUNIT_ASSERT(false); //id_signal_to_find in all blocks of escalator type not found
};
 
void test_insert()
{
g_metro_escalator_types.insert(g_metro_escalator_types.end(), esc_types_container::pair_esc_types(3, 
																										metro_escalator_type(3,"third type of escalators")
																										)
								);




g_metro_escalator_types.insert(g_metro_escalator_types.begin(), esc_types_container::pair_esc_types(1, 
																										metro_escalator_type(1,"first type of escalators")
																										)
								);

g_metro_escalator_types.insert(g_metro_escalator_types.upper_bound(2), esc_types_container::pair_esc_types(2, 
																										metro_escalator_type(2,"second type of escalators")
																										)
							 );

esc_types_container::iterator_esc_types tmp_iter;
tmp_iter=g_metro_escalator_types.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_metro_escalator_types.end());

tmp_iter->second.blocks_insert(tmp_iter->second.blocks_end(),
									metro_escalator_type::pair_blocks (
											3,
											escalator_block(  3,
											"block 3 of esc type 1"	)								
											)
									);

tmp_iter->second.blocks_insert(tmp_iter->second.blocks_begin(),
									metro_escalator_type::pair_blocks (
											1,
											escalator_block(  1,
											"block 1 of esc type 1"	)								
											)
									);

tmp_iter->second.blocks_insert(tmp_iter->second.blocks_upper_bound(2),
									metro_escalator_type::pair_blocks (
											2,
											escalator_block(  2,
											"block 2 of esc type 1"	)								
											)
									);

CPPUNIT_ASSERT( tmp_iter->second.blocks_size()==3);

metro_escalator_type::iterator_blocks tmp_iter_blocks=tmp_iter->second.blocks_find(1);
CPPUNIT_ASSERT( tmp_iter_blocks!=tmp_iter->second.blocks_end());

tmp_iter->second.signals_insert(tmp_iter->second.signals_end(),
									metro_escalator_type::pair_signals 
											(3,
											escalator_signal(  3,
																		"sig3",
																		"signal 3 of block 1 of esc type 1"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(3);

tmp_iter->second.signals_insert(tmp_iter->second.signals_begin(),
									metro_escalator_type::pair_signals 
											(1,
											escalator_signal(  1,
																		"sig1",
																		"signal 1 of block 1 of esc type 1"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(1);

tmp_iter->second.signals_insert(tmp_iter->second.signals_upper_bound(2),
									metro_escalator_type::pair_signals 
											(2,
											escalator_signal(  2,
																		"sig2",
																		"signal 2 of block 1 of esc type 1"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(2);

tmp_iter_blocks=tmp_iter->second.blocks_find(2);
CPPUNIT_ASSERT( tmp_iter_blocks!=tmp_iter->second.blocks_end());

tmp_iter->second.signals_insert(tmp_iter->second.signals_upper_bound(4),
									metro_escalator_type::pair_signals 
											(4,
											escalator_signal(  4,
																		"sig4",
																		"signal 4 of block 2 of esc type 1"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(4);

tmp_iter->second.signals_insert(tmp_iter->second.signals_upper_bound(5),
									metro_escalator_type::pair_signals 
											(5,
											escalator_signal(  5,
																		"sig5",
																		"signal 5 of block 2 of esc type 1"
																	)
											)
									);

tmp_iter_blocks->second.signals_id_push_back(5);


msg_dict_container* msg_container=tmp_iter->second.get_messages();
msg_container->insert(msg_container->end(), msg_dict_container::msg_dict_pair(1, 
																										dictionary_message(1, 1,"message 1 of esc type 1")
																										)
									);
									
msg_container->insert(msg_container->end(), msg_dict_container::msg_dict_pair(2, 
																										dictionary_message(2, 2,"message 2 of esc type 1")
																										)
									);

msg_container->insert(msg_container->end(), msg_dict_container::msg_dict_pair(3, 
																										dictionary_message(3, 2,"message 3 of esc type 1")
																										)
									);

tmp_iter=g_metro_escalator_types.find(2);
CPPUNIT_ASSERT( tmp_iter!=g_metro_escalator_types.end());

tmp_iter->second.blocks_insert(tmp_iter->second.blocks_end(),
									metro_escalator_type::pair_blocks (
											1,
											escalator_block(  1,
											"block 1 of esc type 2"	)								
											)
									);

tmp_iter_blocks=tmp_iter->second.blocks_find(1);
CPPUNIT_ASSERT( tmp_iter_blocks!=tmp_iter->second.blocks_end());

tmp_iter->second.signals_insert(tmp_iter->second.signals_end(),
									metro_escalator_type::pair_signals 
											(1,
											escalator_signal(  1,
																		"sig1",
																		"signal 1 of block 1 of esc type 2"
																	)
											)
									);
tmp_iter_blocks->second.signals_id_push_back(1);


};


void test_erase()
{
esc_types_container::iterator_esc_types tmp_iter;
metro_escalator_type::iterator_blocks tmp_iter_blocks;
metro_escalator_type::iterator_signals tmp_iter_signals;

CPPUNIT_ASSERT(!g_metro_escalator_types.empty());
CPPUNIT_ASSERT(g_metro_escalator_types.size()==3);

tmp_iter=g_metro_escalator_types.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_metro_escalator_types.end());

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

g_metro_escalator_types.erase(tmp_iter);

tmp_iter=g_metro_escalator_types.find(2);
CPPUNIT_ASSERT( tmp_iter!=g_metro_escalator_types.end());
g_metro_escalator_types.erase(tmp_iter);

tmp_iter=g_metro_escalator_types.find(3);
CPPUNIT_ASSERT( tmp_iter!=g_metro_escalator_types.end());
g_metro_escalator_types.erase(tmp_iter);

CPPUNIT_ASSERT(g_metro_escalator_types.empty());
CPPUNIT_ASSERT(g_metro_escalator_types.size()==0);
};

void test_load()
{
//ATTENTION  - all of that messages types must be in file_name !!!
/*
#example of right file_name file
[type]
id=1
name=first type of escalators

	[block]
	id=1
	name=block 1 of esc type 1

			[signal]
			index=1
			name=sig1
			hint=signal 1 of block 1 of esc type 1

			[signal]
			index=3
			name=sig3
			hint=signal 3 of block 1 of esc type 1

	[block]
	id=2
	name=block 2 of esc type 1

			[signal]
			index=2
			block=1
			name=sig2
			hint=signal 2 of block 1 of esc type 1

			[signal]
			index=4
			name=sig4
			hint=signal 4 of block 2 of esc type 1


	[message]
	id=1
	message type=1
	text=message 1 of esc type 1

	[message]
	id=2
	message type=2
	text=message 2 of esc type 1

[type]
id=2
name=second type of escalators

	[block]
	id=1
	name=block 1 of esc type 2

			[signal]
			index=5
			block=2
			type=1
			name=sig5
			hint=signal 5 of block 2 of esc type 1

			[signal]
			index=1
			name=sig1
			hint=signal 1 of block 1 of esc type 2

	[block]
	id=3
	type=1
	name=block 3 of esc type 1
	
	[message]
	id=3
	message type=2
	type=1
	text=message 3 of esc type 1


[type]
id=3
name=third type of escalators
*/
esc_types_container::iterator_esc_types tmp_iter1, tmp_iter2;

test_insert();
esc_types_container tmp_esc_types_container;
tmp_esc_types_container.load(&g_system_settings, &g_msg_types, file_name);

CPPUNIT_ASSERT(!tmp_esc_types_container.empty());
CPPUNIT_ASSERT(!g_metro_escalator_types.empty());
CPPUNIT_ASSERT(g_metro_escalator_types.size()==tmp_esc_types_container.size());

tmp_iter1=tmp_esc_types_container.begin();
while (tmp_iter1!=tmp_esc_types_container.end())
	{
		tmp_iter2=g_metro_escalator_types.find(tmp_iter1->second.get_id());
		CPPUNIT_ASSERT(tmp_iter2!=g_metro_escalator_types.end());		

		CPPUNIT_ASSERT(tmp_iter1->second.get_id()==tmp_iter2->second.get_id());		
		CPPUNIT_ASSERT(tmp_iter1->second.get_name().compare(tmp_iter2->second.get_name()) == 0);		
		CPPUNIT_ASSERT(tmp_iter1->second.blocks_size()==tmp_iter2->second.blocks_size());		
		CPPUNIT_ASSERT(tmp_iter1->second.signals_size()==tmp_iter2->second.signals_size());		

		metro_escalator_type::iterator_blocks tmp_iter_blocks1, tmp_iter_blocks2;
		tmp_iter_blocks1=tmp_iter1->second.blocks_begin();
		while (tmp_iter_blocks1!=tmp_iter1->second.blocks_end())
			{
				tmp_iter_blocks2=tmp_iter2->second.blocks_find(tmp_iter_blocks1->second.get_id());
				CPPUNIT_ASSERT(tmp_iter_blocks2!=tmp_iter2->second.blocks_end());
				CPPUNIT_ASSERT(tmp_iter_blocks2->second.get_id()==tmp_iter_blocks1->second.get_id());
				CPPUNIT_ASSERT(tmp_iter_blocks2->second.get_name().compare(tmp_iter_blocks1->second.get_name())==0);
				CPPUNIT_ASSERT(tmp_iter_blocks2->second.signals_id_size ()== tmp_iter_blocks1->second.signals_id_size ());

				escalator_block::iterator_signals_id tmp_iter_signals_id1=tmp_iter_blocks1->second.signals_id_begin(), 
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

		metro_escalator_type::iterator_signals tmp_iter_signals1=tmp_iter1->second.signals_begin(),
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
	file_name="../unit_tests/esc_types_test.ini";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Tests for Container of Metro Escalators Types ");

suite_of_tests->addTest(new CppUnit::TestCaller<esc_types_container_test> 
												( "insert metod test",
													&esc_types_container_test::test_insert)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<esc_types_container_test> 
												( "erase metod test",
													&esc_types_container_test::test_erase)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<esc_types_container_test> 
												( "load metod test",
													&esc_types_container_test::test_load)
										);


return suite_of_tests;
}
};

#endif