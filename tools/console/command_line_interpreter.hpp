//  Boost cli library command_line_interpreter.hpp header file  --------------//

//  (C) Copyright Jean-Daniel Michaud 2007. Permission to copy, use, modify, 
//  sell and distribute this software is granted provided this copyright notice 
//  appears in all copies. This software is provided "as is" without express or 
//  implied warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org/LICENSE_1_0.txt for licensing.
//  See http://code.google.com/p/clipo/ for library home page.

//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : May 7, 2014
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
// CASROS imported Clipo on May 7, 2014.
//

#ifndef BOOST_COMMAND_LINE_INTERPRETER
#define BOOST_COMMAND_LINE_INTERPRETER

#include <vector>
#include <string>
#include <iostream>
#include <iterator>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include "color_console.h"

#if 0 // ncurse test
struct Snake{
    int x, y;
    char s = 'O'; // logo
} snake;
// to support arrow
#if SC_ON_WINDOWS
#include <conio.h>
#else
#include <curses.h>
#endif
#endif

namespace boost { namespace cli {

typedef boost::program_options::options_description commands_description;

/*
 * Unshamefully copied from the split_winmain function developed by Vladimir Prus 
 */ 
static std::vector<std::string> split_command_line(const std::string& input)
{
  std::vector<std::string> result;

  std::string::const_iterator i = input.begin(), e = input.end();
  for(;i != e; ++i)
    if (!isspace((unsigned char)*i))
      break;
 
  if (i != e) 
  {
    std::string current;
    bool inside_quoted = false;
    int backslash_count = 0;
    
    for(; i != e; ++i) 
    {
      if (*i == '"') 
      {
        // '"' preceded by even number (n) of backslashes generates
        // n/2 backslashes and is a quoted block delimiter
        if (backslash_count % 2 == 0) 
        {
          current.append(backslash_count / 2, '\\');
          inside_quoted = !inside_quoted;
          // '"' preceded by odd number (n) of backslashes generates
          // (n-1)/2 backslashes and is literal quote.
        } 
        else 
        {
          current.append(backslash_count / 2, '\\');                
          current += '"';                
        }
        backslash_count = 0;
      } 
      else if (*i == '\\') 
      {
        ++backslash_count;
      } else 
      {
        // Not quote or backslash. All accumulated backslashes should be
        // added
        if (backslash_count) 
        {
          current.append(backslash_count, '\\');
          backslash_count = 0;
        }
        if (isspace((unsigned char)*i) && !inside_quoted) 
        {
          // Space outside quoted section terminate the current argument
          result.push_back(current);
          current.resize(0);
          for(;i != e && isspace((unsigned char)*i); ++i)
            ;
          --i;
        } 
        else 
        {                  
          current += *i;
        }
      }
    }

    // If we have trailing backslashes, add them
    if (backslash_count)
      current.append(backslash_count, '\\');

    // If we have non-empty 'current' or we're still in quoted
    // section (even if 'current' is empty), add the last token.
    if (!current.empty() || inside_quoted)
      result.push_back(current);        
  }
  return result;
}

class command_line_interpreter
{
public:
  command_line_interpreter(const commands_description& desc)
  {
    m_desc = &desc;
  }

  command_line_interpreter(const commands_description& desc, 
                           const std::string &prompt)
  {
    m_desc = &desc;
    m_prompt = prompt;
  }

  void handle_read_line(std::string line)
  {
    std::vector<std::string> args;
    
    // huu, ugly...
    args = split_command_line(std::string("--") + line); 
    
    try
    {
      boost::program_options::variables_map vm;
      boost::program_options::store(
        boost::program_options::command_line_parser(args).options(*m_desc).run(), 
        vm);
      boost::program_options::notify(vm);
    }
    catch (boost::program_options::unknown_option &e) 
    {
      std::cerr << "error: " << e.what() << std::endl;
    }
    catch (boost::program_options::invalid_command_line_syntax &e)
    {
      std::cerr << "error: " << e.what() << std::endl;
    }
    catch (boost::program_options::validation_error &e)
    {
      std::cerr << "error: " << e.what() << std::endl;
    }
  }

  void interpret(std::istream &input_stream)
  {
    std::string command;
    std::cout << m_prompt << std::flush;

#if 1
    while (std::getline(input_stream, command, '\n'))
    {
      handle_read_line(command);
      std::cout << m_prompt << std::flush;
    }

#else // Ncurses test code
#if 0
    int KB_code=0;

    echo();
    std::cout << __LINE__ << std::endl;

    WINDOW *w;
    char c;

    w = initscr();
    cbreak();	/* Line buffering disabled. pass on everything */
    keypad(stdscr, TRUE);

   while (true)
   { 
       //if (kbhit())
       {
           KB_code = getch();
           if (KB_code == ERR)
               continue;

           std::cout << "input: " << KB_code << std::endl;
           switch (KB_code)
           {
           case 27://ESC:
               std::cout << "ESC" << std::endl;
               break;

           //case 75://KB_LEFT:
           case KB_LEFT:
               std::cout << "LEFT" << std::endl;
               break;

           case 77://KB_RIGHT:
               std::cout << "RIGHT" << std::endl;
               break;

           case 72://KB_UP:
               std::cout << "UP" << std::endl;
               break;

           case 80://KB_DOWN:
               std::cout << "DOWN" << std::endl;
               break;

           }        

       }
   }
#endif

#if 0
   initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, true);
    nodelay(stdscr, true);
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK );
    attron(COLOR_PAIR(1));
    int HEIGHT, WIDTH;

    getmaxyx(stdscr, HEIGHT, WIDTH);

    for (int x = 0; x < WIDTH-1; x++)
        mvaddch(0, x, '*');

    for (int y = 0; y < HEIGHT-2; y++)
        mvaddch(y, WIDTH-1, '*');

    for (int x = 0; x < WIDTH-1; x++)
        mvaddch(HEIGHT-2, x, '*');

    for (int y = 0; y < HEIGHT-2; y++)
        mvaddch(y, 0, '*');


    snake.x = WIDTH/2;
    snake.y = HEIGHT/2;
    mvaddch(snake.y, snake.x, snake.s);
    refresh();


    int key;
    while((key = getch()) != 'q')
    {
        mvaddch(snake.y, snake.x, ' ');
        switch(key)
        {
        case KEY_RIGHT:
            snake.x +=1;    
            break;

        case KEY_LEFT:
            snake.x -=1;    
            break;

        case KEY_UP:
            snake.y -=1;    
            break;

        case KEY_DOWN:
            snake.y +=1; 
            break;
        }

        mvaddch(snake.y, snake.x, snake.s);

        //usleep(100000);
        refresh();
    }

    getch();
    erase();
    endwin();
#endif

#if 0
     int i = 0;

    initscr();

    scrollok(stdscr,TRUE);

    while(1)
    {
        printw("%d - lots and lots of lines flowing down the terminal\n", i);
        ++i;
        refresh();
        usleep(100000);
    }

    endwin();
#endif
#endif
  }

  typedef char *(*realine_function_pointer_t)(const char *);

  void interpret_(std::istream &input_stream, 
                  realine_function_pointer_t f)
  {
    char        *line = NULL;

    // MJ: to suppress warning (Apple LLVM ver 5.1 (clang-503.0.40)
    //while (line = boost::bind(f, m_prompt.c_str())())
    while ( (line = boost::bind(f, m_prompt.c_str())()) )
    {
      if (!line)
        continue ;
        
      std::string command = line;
      free(line);
      handle_read_line(command);
    }
  }
  

private:
  const commands_description* m_desc;
  std::string                 m_prompt;
};

} // !namespace cli
} // !namespace boost

#endif // !BOOST_COMMAND_LINE_INTERPRETER
