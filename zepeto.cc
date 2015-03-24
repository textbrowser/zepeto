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

#include <iostream>
#include <string>

#include "zepeto.h"

zepeto::zepeto(void)
{
  m_product_file = "zepeto.table";

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
	    << "Incorrect usage. Please see the manual.\n";
}

int main(int argc, char *argv[])
{
  int rc = EXIT_SUCCESS;
  zepeto *z = 0;

  if(argc <= 1 || !argv)
    {
      rc = EXIT_FAILURE;
      zepeto::print_help();
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

      if(z)
	delete z;

      goto done_label;
    }

  for(int i = 0; i < argc; i++)
    if(argv[i] && strcmp(argv[i], "-a") == 0)
      {
	z->print_about();
	goto done_label;
      }

  delete z;

 done_label:
  return rc;
}
