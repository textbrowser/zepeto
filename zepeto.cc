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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
}

#include <fstream>
#include <iostream>
#include <set>
#include <string>

#include "zepeto.h"

zepeto::zepeto(void)
{
  m_product_file = "zepeto.table";
  m_quiet = false;

  struct passwd *pw = getpwuid(getuid());

  if(pw)
    m_tempdir = pw->pw_dir;
  else
    m_tempdir = "/tmp";
}

zepeto::~zepeto()
{
}

std::string zepeto::product_file(void) const
{
  return m_product_file;
}

void zepeto::add_detach_product(const char *product)
{
  if(!product || strlen(product) == 0)
    return;
  else if(m_detached_products.count(product) == 0)
    m_detached_products.insert(product);
}

void zepeto::add_use_product(const char *product)
{
  if(!product || strlen(product) == 0)
    return;
  else if(m_used_products.count(product) == 0)
    m_used_products.insert(product);
}

void zepeto::engage(void) const
{
  std::ifstream file;
  std::string line;

  file.open(m_product_file.c_str(), std::ios::in);

  if(file.is_open())
    while(getline(file, line))
      {
	if(line.find("#") == 0)
	  continue;
	else if(line.find("description") != std::string::npos)
	  continue;

	size_t index = line.find(".");

	if(index == std::string::npos)
	  continue;

	line = line.substr(0, index);

	if(line.empty())
	  continue;

	if(m_detached_products.count(line) > 0)
	  {
	  }
      }

  file.close();
}

void zepeto::list_products(void) const
{
  std::ifstream file;
  std::set<std::string> s;
  std::string line;

  file.open(m_product_file.c_str(), std::ios::in);

  if(file.is_open())
    while(getline(file, line))
      {
	if(line.find("#") == 0)
	  continue;

	size_t index = line.find(".");

	if(index == std::string::npos)
	  continue;

	line = line.substr(0, index);

	if(line.empty())
	  continue;

	if(!s.count(line))
	  s.insert(line);
      }

  for(std::set<std::string>::iterator it = s.begin(); it != s.end(); ++it)
    std::cout << *it << "\n";

  file.close();
}

void zepeto::print_about(void) const
{
  std::cout << "zepeto\n"
	    << "Version: " << ZEPETO_VERSION << ".\n"
	    << "Product file: " << product_file() << ".\n"
	    << "Temporary directory: " << m_tempdir << ".\n";
}

void zepeto::print_help(void)
{
  std::cerr << "zepeto\n"
	    << "Incorrect usage. Please read the manual.\n";
}

void zepeto::set_product_file(const char *product_file)
{
  if(product_file)
    m_product_file = product_file;
}

void zepeto::set_quiet(const bool quiet)
{
  m_quiet = quiet;
}

int main(int argc, char *argv[])
{
  int rc = EXIT_SUCCESS;
  zepeto *z = 0;

  if(argc <= 1 || !argv)
    {
      rc = EXIT_FAILURE;
      goto done_label;
    }

  try
    {
      z = new zepeto();
    }
  catch(std::bad_alloc &)
    {
      rc = EXIT_FAILURE;
      goto done_label;
    }
  catch(...)
    {
      rc = EXIT_FAILURE;
      goto done_label;
    }

  for(int i = 1; i < argc; i++)
    if(argv[i] && strcmp(argv[i], "-q") == 0)
      z->set_quiet(true);
    else if(argv[i] && strcmp(argv[i], "-t") == 0)
      {
	i += 1;

	if(i < argc && argv[i])
	  z->set_product_file(argv[i]);
	else
	  {
	    rc = EXIT_FAILURE;
	    goto done_label;
	  }

	break;
      }

  for(int i = 1; i < argc; i++)
    if(argv[i] && strcmp(argv[i], "-a") == 0)
      {
	z->print_about();
	goto done_label;
      }
    else if(argv[i] && strcmp(argv[i], "-d") == 0)
      {
	i += 1;

	if(i < argc && argv[i])
	  z->add_detach_product(argv[i]);
	else
	  {
	    rc = EXIT_FAILURE;
	    goto done_label;
	  }
      }
    else if(argv[i] && strcmp(argv[i], "-l") == 0)
      {
	z->list_products();
	goto done_label;
      }
    else if(argv[i] && strcmp(argv[i], "-q") == 0)
      {
      }
    else if(argv[i] && strcmp(argv[i], "-u") == 0)
      {
	i += 1;

	if(i < argc && argv[i])
	  z->add_use_product(argv[i]);
	else
	  {
	    rc = EXIT_FAILURE;
	    goto done_label;
	  }
      }
    else
      {
	rc = EXIT_FAILURE;
	break;
      }

  z->engage();

 done_label:
  delete z;

  if(rc != EXIT_SUCCESS)
    zepeto::print_help();

  return rc;
}
