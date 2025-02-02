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
#include <stdlib.h>
#include <string.h>
}

#include <iostream>
#include <stdexcept>

#include "zepeto.h"

int main(int argc, char *argv[])
{
  if(argc <= 1 || !argv)
    return EXIT_FAILURE;

  auto rc = EXIT_SUCCESS;
  zepeto *z = nullptr;

  try
    {
      for(int i = 1; i < argc; i++)
	if(argv[i] && strcmp(argv[i], "-p") == 0)
	  {
	    z = new zepeto(false);
	    z->purge();
	    delete z;
	    z = nullptr;
	    break;
	  }

      for(int i = 1; i < argc; i++)
	if(argv[i] && strcmp(argv[i], "-t") == 0)
	  {
	    i += 1;

	    if(i < argc && argv[i])
	      {
		z = new zepeto(true);
		z->set_product_file(argv[i]);
		break;
	      }
	    else
	      /*
	      ** Assume the default zepeto.table file.
	      */

	      rc = EXIT_FAILURE;
	  }

      if(!z)
	z = new zepeto(true);

      for(int i = 1; i < argc; i++)
      	if(argv[i] && strcmp(argv[i], "-i") == 0)
	  {
	    z->print_about();
	    goto done_label;
	  }
	else if(argv[i] && strcmp(argv[i], "-l") == 0)
	  {
	    z->print_products();
	    goto done_label;
	  }

      for(int i = 1; i < argc; i++)
	if(argv[i] && strcmp(argv[i], "-a") == 0)
	  {
	    i += 1;

	    if(i < argc && argv[i])
	      z->add_attach_product(argv[i]);
	    else
	      rc = EXIT_FAILURE;
	  }
	else if(argv[i] && strcmp(argv[i], "-d") == 0)
	  {
	    i += 1;

	    if(i < argc && argv[i])
	      z->add_detach_product(argv[i]);
	    else
	      rc = EXIT_FAILURE;
	  }
	else if(argv[i] && strcmp(argv[i], "-i") == 0)
	  {
	  }
	else if(argv[i] && strcmp(argv[i], "-l") == 0)
	  {
	  }
	else if(argv[i] && strcmp(argv[i], "-p") == 0)
	  {
	  }
	else if(argv[i] && strcmp(argv[i], "-s") == 0)
	  {
	    i += 1;

	    if(i < argc && argv[i])
	      z->add_print_product(argv[i]);
	    else
	      rc = EXIT_FAILURE;
	  }
	else if(argv[i] && strcmp(argv[i], "-t") == 0)
	  i += 1;
	else
	  rc = EXIT_FAILURE;

      z->final();
    }
  catch(const std::bad_alloc &exception)
    {
      rc = EXIT_FAILURE;

      if(!z)
	return rc;
    }
  catch(const std::runtime_error &exception)
    {
      rc = EXIT_FAILURE;
      std::cerr << exception.what();
    }
  catch(...)
    {
    }

 done_label:

  auto remove_temporary_file = false;

  try
    {
      if(z && z->has_error())
	{
	  rc = EXIT_FAILURE;
	  z->print_error();
	}
    }
  catch(const std::runtime_error &exception)
    {
      rc = EXIT_FAILURE;
      remove_temporary_file = true;
      std::cerr << exception.what();
    }
  catch(...)
    {
      rc = EXIT_FAILURE;
      remove_temporary_file = true;
    }

  if(remove_temporary_file && z)
    z->remove_temporary_file();

  delete z;
  return rc;
}
