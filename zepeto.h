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
  void add_attach_product(const char *product);
  void add_detach_product(const char *product);
  void engage(void) const;
  void list_products(void) const;
  void print_about(void) const;
  void set_product_file(const char *print_file);
  void set_quiet(const bool quiet);

 private:
  bool m_quiet;
  std::set<std::string> m_attached_products;
  std::set<std::string> m_detached_products;
  std::string m_output;
  std::string m_product_file;
  std::string m_tempdir;
  void attach_paths(const std::string &paths) const;
  void detach_paths(const std::string &paths) const;
};

#endif
