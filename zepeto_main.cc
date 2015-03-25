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

#include "zepeto.h"

int main(int argc, char *argv[])
{
  if(argc <= 1 || !argv)
    return EXIT_FAILURE;

  int rc = EXIT_SUCCESS;
  zepeto *z = 0;

  try
    {
      z = new zepeto();
    }
  catch(std::bad_alloc &)
    {
      rc = EXIT_FAILURE;
      return rc;
    }
  catch(...)
    {
      rc = EXIT_FAILURE;
      goto done_label;
    }

  for(int i = 1; i < argc; i++)
    if(argv[i] && strcmp(argv[i], "-t") == 0)
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
	i += 1;

	if(i < argc && argv[i])
	  z->add_attach_product(argv[i]);
	else
	  {
	    rc = EXIT_FAILURE;
	    goto done_label;
	  }
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
    else if(argv[i] && strcmp(argv[i], "-i") == 0)
      {
	z->print_about();
	goto done_label;
      }
    else if(argv[i] && strcmp(argv[i], "-l") == 0)
      {
	z->print_products();
	goto done_label;
      }
    else if(argv[i] && strcmp(argv[i], "-t") == 0)
      {
	i += 1;
      }
    else
      {
	rc = EXIT_FAILURE;
	goto done_label;
      }

  z->final();

 done_label:

  if(rc != EXIT_SUCCESS)
    if(z)
      z->print_help();

  delete z;
  return rc;
}
