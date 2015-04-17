#ifndef _zepeto_h_
#define _zepeto_h_

#define ZEPETO_VERSION "1.01"

#include <set>
#include <string>

class zepeto
{
 public:
  zepeto(void);
  ~zepeto();
  bool has_error(void) const;
  void add_attach_product(const char *product);
  void add_detach_product(const char *product);
  void final(void);
  void print_about(void);
  void print_error(void);
  void print_products(void);
  void remove_temporary_file(void);
  void set_product_file(const char *print_file);

 private:
  char *m_buffer;
  char *m_tempfilename;
  int m_fd;
  static const int ATTACH = 0;
  static const int DETACH = 1;
  std::set<std::string> m_attached_products;
  std::set<std::string> m_detached_products;
  std::string m_error;
  std::string m_output;
  std::string m_product_file;
  std::string m_tempdir;
  void action(const int a, const std::string &paths);
};

#endif
