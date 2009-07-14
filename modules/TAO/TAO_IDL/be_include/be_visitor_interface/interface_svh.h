// $Id$

// ============================================================================
//
// = LIBRARY
//    TAO_IDL_BE
//
// = FILENAME
//    interface_svh.h
//
// = DESCRIPTION
//    Concrete visitor for the Interface node.
//    This provides for code generation in the CIAO
//    servant header
//
// = AUTHOR
//   Jeff Parsons <j.parsons@vanderbilt.edu>
//
// ============================================================================

#ifndef _BE_INTERFACE_INTERFACE_SVH_H_
#define _BE_INTERFACE_INTERFACE_SVH_H_

class be_visitor_interface_svh : public be_visitor_interface
{
  //
  // = TITLE
  //   be_visitor_interface_svh
  //
  // = DESCRIPTION
  //   This is a concrete visitor to generate the
  //   CIAO servant header for interface
  //
  //
public:
  be_visitor_interface_svh (be_visitor_context *ctx);

  ~be_visitor_interface_svh (void);

  virtual int visit_interface (be_interface *node);
  virtual int visit_operation (be_operation *node);
  virtual int visit_attribute (be_attribute *node);
};

#endif /* _BE_INTERFACE_INTERFACE_SVH_H_ */
