#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

class		dictionary_message
{
private:
	int			number;
	string	text;
	PgColor_t	color;
	
	dictionary_message();
public:
	dictionary_message(int new_number, string new_text, PgColor_t new_color) : number(new_number), text(new_text) , color(new_color) {};
	PgColor_t get_color(){ return (color);}
	string get_text () {return (text);}
	int get_id () {return( number);}
};

#endif
