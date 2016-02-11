/*
** Copyright (c) Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
** notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
** notice, this list of conditions and the following disclaimer in the
** documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
** derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

extern "C"
{
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
}

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>

#include "zepeto.h"

zepeto::zepeto(void)
{
  m_buffer = new char[1024];
  m_product_file = "/usr/local/share/zepeto.table";
  m_tempfilename = 0;

  struct passwd *pw = getpwuid(getuid());

  if(pw)
    m_tempdir = pw->pw_dir;
  else
    m_tempdir = "/tmp";

  std::ostringstream stream;

  stream << m_tempdir
	 << "/.zepeto.sourceme."
	 << getuid()
	 << "."
	 << getpid()
	 << "XXXXXX";

  size_t length = stream.str().length() + 1;

  m_tempfilename = new char[length];
  memset(m_tempfilename, 0, length);
  strncpy(m_tempfilename, stream.str().c_str(), length - 1);

  if((m_fd = mkstemp(m_tempfilename)) == -1)
    m_error.append("echo \"mkstemp() failure.\"\n");
}

zepeto::~zepeto()
{
  close(m_fd);
  delete []m_buffer;
  delete []m_tempfilename;
}

bool zepeto::has_error(void) const
{
  return !m_error.empty();
}

void zepeto::action(const int a, const std::string &string)
{
  try
    {
      if(string.empty())
	return;
      else if(string.find(":") == std::string::npos)
	return;

      std::string paths;
      std::string variable;

      /*
      ** VARIABLE:p1:p2:...:pn
      */

      paths = string.substr(string.find(":") + 1);
      variable = string.substr(0, string.find(":"));

      if(paths.empty() || variable.empty())
	return;

      std::set<std::string> set;

      do
	{
	  size_t index = paths.find(":");
	  std::string p;

	  if(index == std::string::npos)
	    p = paths;
	  else
	    p = paths.substr(0, index);

	  if(p.empty() || paths.empty())
	    break;
	  else
	    {
	      if(index == std::string::npos)
		paths.clear();
	      else
		paths = paths.substr(index + 1);

	      struct stat sb;

	      if(stat(p.c_str(), &sb) != 0)
		{
		  m_output.append("echo \"The path ");
		  m_output.append(p);
		  m_output.append(" is not accessible.\"\n");
		}
	      else if(set.count(p) == 0)
		set.insert(p);
	    }
	}
      while(true);

      std::string attached;
      std::string e;

      if(m_variables.count(variable) != 0)
	e = m_variables[variable];
      else
	{
	  char *temp = std::getenv(variable.c_str());

	  if(temp)
	    e = temp;
	}

      for(std::set<std::string>::iterator it = set.begin(); it != set.end();
	  ++it)
	if(a == ATTACH)
	  {
	    if(e.find(*it) == std::string::npos)
	      {
		attached.append(*it);
		attached.append(":");
	      }
	  }
	else if(a == DETACH)
	  {
	    size_t index = e.find(":" + *it + ":");

	    if(index != std::string::npos)
	      /*
	      ** The directory is between other directories.
	      */

	      e.replace(index, (*it).length() + 2, ":");
	    else
	      {
		if(e.substr(0, (*it).length() + 1) == (*it) + ":")
		  /*
		  ** The directory is at the beginning.
		  */

		  e.erase(0, (*it).length() + 1);
		else
		  {
		    index = e.find(":" + *it);

		    if(index != std::string::npos &&
		       e.length() == index + (*it).length() + 1)
		      /*
		      ** The directory is at the end.
		      */

		      e.erase(index, (*it).length() + 1);
		    else if(e == *it)
		      e.clear();
		  }
	      }

	    do
	      {
		index = e.find("::");

		if(index == std::string::npos)
		  break;
		else
		  e.replace(index, 2, ":");
	      }
	    while(true);

	    if(e.substr(0, 1) == ":")
	      e.erase(0, 1);

	    if(e.length() > 0 && e.substr(e.length() - 1, 1) == ":")
	      e.erase(e.length() - 1, 1);
	  }

      if(a == ATTACH)
	{
	  if(!attached.empty())
	    {
	      if(e.empty())
		attached.erase(attached.length() - 1, 1); // Remove :.

	      m_variables[variable] = attached + e;
	    }
	}
      else if(a == DETACH)
	{
	  if(e.empty())
	    m_variables[variable].clear();
	  else
	    m_variables[variable] = e;
	}
    }
  catch(const std::bad_alloc &exception)
    {
      throw exception;
    }
  catch(...)
    {
      m_error.append("echo \"An error occurred within action().\"\n");
      throw std::runtime_error(m_error);
    }
}

void zepeto::add_attach_product(const char *product)
{
  try
    {
      if(!product || strlen(product) == 0)
	return;
      else if(m_attached_products.count(product) == 0)
	m_attached_products.insert(product);
    }
  catch(const std::bad_alloc &exception)
    {
      throw exception;
    }
  catch(...)
    {
      m_error.append("echo \"add_attach_product() error.\"\n");
      throw std::runtime_error(m_error);
    }
}

void zepeto::add_detach_product(const char *product)
{
  try
    {
      if(!product || strlen(product) == 0)
	return;
      else if(m_detached_products.count(product) == 0)
	m_detached_products.insert(product);
    }
  catch(const std::bad_alloc &exception)
    {
      throw exception;
    }
  catch(...)
    {
      m_error.append("echo \"add_detach_product() error.\"\n");
      throw std::runtime_error(m_error);
    }
}

void zepeto::final(void)
{
  if(!m_buffer)
    throw std::runtime_error("m_buffer is empty.");

  try
    {
      if(!m_error.empty())
	return;

      std::ifstream file;

      file.open(m_product_file.c_str(), std::ios::in);

      if(!file.is_open())
	{
	  m_error.append("echo \"Error opening the zepeto.table file.\"\n");
	  return;
	}

      std::set<std::string> attached_products(m_attached_products);
      std::set<std::string> detached_products(m_detached_products);

      if(file.is_open())
	while(file.getline(m_buffer, 1024))
	  {
	    size_t first = 0;
	    size_t last = 0;
	    std::string line(m_buffer);

	    first = line.find_first_not_of(' ');
	    last = line.find_last_not_of(' ');

	    if(first != std::string::npos &&
	       last != std::string::npos)
	      line = line.substr(first, last - first + 1);

	    if(line.find("#") == 0)
	      continue;
	    else if(line.find("description") != std::string::npos)
	      continue;

	    size_t index = line.find(".");

	    if(index == std::string::npos)
	      continue;

	    std::string path;

	    path = line.substr(index + 1);
	    line = line.substr(0, index);

	    if(line.empty())
	      continue;

	    if(m_attached_products.count(line) > 0)
	      {
		action(ATTACH, path);
		attached_products.erase(line);
	      }

	    if(m_detached_products.count(line) > 0)
	      {
		action(DETACH, path);
		detached_products.erase(line);
	      }
	  }

      file.close();

      {
	std::set<std::string>::iterator it;

	for(it = attached_products.begin();
	    it != attached_products.end(); ++it)
	  {
	    m_error.append("echo \"The product ");
	    m_error.append(*it);
	    m_error.append(" does not exist.\"\n");
	  }

	for(it = detached_products.begin();
	    it != detached_products.end(); ++it)
	  {
	    m_error.append("echo \"The product ");
	    m_error.append(*it);
	    m_error.append(" does not exist.\"\n");
	  }
      }

      if(m_error.empty() && m_fd != -1)
	{
	  std::map<std::string, std::string>::iterator it;

	  for(it = m_variables.begin(); it != m_variables.end(); ++it)
	    {
	      std::string k(it->first);
	      std::string v(it->second);

	      if(v.empty())
		{
		  m_output.append("unset ");
		  m_output.append(k);
		  m_output.append("\n");
		}
	      else
		{
		  m_output.append("export ");
		  m_output.append(k);
		  m_output.append("=");
		  m_output.append(v);
		  m_output.append("\n");
		}
	    }

	  if(!m_output.empty())
	    {
	      write(m_fd, m_output.c_str(), m_output.length());
	      fsync(m_fd);
	    }
	}

      if(m_error.empty())
	if(m_tempfilename)
	  std::cout << m_tempfilename;
    }
  catch(const std::bad_alloc &exception)
    {
      throw exception;
    }
  catch(...)
    {
      m_error.append("echo \"An error occurred within final().\"\n");
      throw std::runtime_error(m_error);
    }
}

void zepeto::print_about(void)
{
  try
    {
      m_output.clear();
      m_output.append("echo \"zepeto\"\n");
      m_output.append("echo \"Version: ");
      m_output.append(ZEPETO_VERSION);
      m_output.append(".\"\n");
      m_output.append("echo \"Product file: ");
      m_output.append(m_product_file);
      m_output.append(".\"\n");
      m_output.append("echo \"Temporary directory: ");
      m_output.append(m_tempdir);
      m_output.append(".\"\n");

      if(m_fd != -1)
	{
	  write(m_fd, m_output.c_str(), m_output.length());
	  fsync(m_fd);
	}

      if(m_tempfilename)
	std::cout << m_tempfilename;
    }
  catch(const std::bad_alloc &exception)
    {
      throw exception;
    }
  catch(...)
    {
      m_error.append("echo \"An error occurred within print_about().\"\n");
      throw std::runtime_error(m_error);
    }
}

void zepeto::print_error(void)
{
  try
    {
      if(m_fd != -1)
	{
	  write(m_fd, m_error.c_str(), m_error.length());
	  fsync(m_fd);
	}

      if(m_tempfilename)
	std::cout << m_tempfilename;
    }
  catch(const std::bad_alloc &exception)
    {
      throw exception;
    }
  catch(...)
    {
      throw std::runtime_error("An error occurred within print_error().\n");
    }
}

void zepeto::print_products(void)
{
  if(!m_buffer)
    throw std::runtime_error("m_buffer is empty.");

  try
    {
      std::ifstream file;

      file.open(m_product_file.c_str(), std::ios::in);

      if(!file.is_open())
	{
	  m_error.append("echo \"Error opening the zepeto.table file.\"\n");
	  return;
	}

      std::set<std::string> set;

      while(file.getline(m_buffer, 1024))
	{
	  size_t first = 0;
	  size_t last = 0;
	  std::string line(m_buffer);

	  first = line.find_first_not_of(' ');
	  last = line.find_last_not_of(' ');

	  if(first != std::string::npos &&
	     last != std::string::npos)
	    line = line.substr(first, last - first + 1);

	  if(line.find("#") == 0)
	    continue;

	  size_t index = line.find(".");

	  if(index == std::string::npos)
	    continue;

	  line = line.substr(0, index);

	  if(line.empty())
	    continue;

	  if(set.count(line) == 0)
	    set.insert(line);
	}

      file.close();
      m_output.clear();

      for(std::set<std::string>::iterator it = set.begin(); it != set.end();
	  ++it)
	{
	  m_output.append("echo \"");
	  m_output.append(*it);
	  m_output.append("\"\n");
	}

      if(m_fd != -1)
	if(!m_output.empty())
	  {
	    write(m_fd, m_output.c_str(), m_output.length());
	    fsync(m_fd);
	  }

      if(m_tempfilename)
	std::cout << m_tempfilename;
    }
  catch(const std::bad_alloc &exception)
    {
      throw exception;
    }
  catch(...)
    {
      m_error.append
	("echo \"An error occurred within print_products().\"\n");
      throw std::runtime_error(m_error);
    }
}

void zepeto::remove_temporary_file(void)
{
  if(m_tempfilename)
    std::remove(m_tempfilename);
}

void zepeto::set_product_file(const char *product_file)
{
  try
    {
      if(product_file)
	m_product_file = product_file;
    }
  catch(const std::bad_alloc &exception)
    {
      throw exception;
    }
  catch(...)
    {
      m_error.append
	("echo \"An error occurred within set_product_file().\"\n");
      throw std::runtime_error(m_error);
    }
}
