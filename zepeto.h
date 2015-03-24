#ifndef _zepeto_h_
#define _zepeto_h_

#define ZEPETO_VERSION "1.00"

class zepeto
{
 public:
  zepeto(void);
  ~zepeto();
  static void print_help(void);
  std::string product_file(void) const;
  void list_products(void) const;
  void print_about(void) const;
  void set_product_file(const char *print_file);
  void set_quiet(const bool quiet);

 private:
  bool m_quiet;
  std::string m_product_file;
  std::string m_tempdir;
};

#endif
