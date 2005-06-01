#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

struct line_description {
 string line_name;
 string line_password;
 string line_script;

  line_description (string new_line_name,
                     string new_line_password,
                     string new_line_script):
  line_name(new_line_name),
  line_password(new_line_password),
  line_script(new_line_script) {};
};

typedef vector<line_description> lines_container;
#endif
