// -*- C++ -*-

//=============================================================================
/**
 *  @file   FT_Storable_Naming_Context.h
 *
 *
 *  @author Kevin Stanley <stanleyk@ociweb.com>
 */
//=============================================================================

#ifndef TAO_FT_STORABLE_NAMING_CONTEXT_H
#define TAO_FT_STORABLE_NAMING_CONTEXT_H
#include /**/ "ace/pre.h"

#include "orbsvcs/Naming/Storable_Naming_Context.h"
#include "orbsvcs/Naming/FaultTolerant/ftnaming_export.h"
#include "orbsvcs/PortableGroupC.h"
#include "orbsvcs/Naming/FaultTolerant/FT_Naming_Manager.h"
#include "orbsvcs/FT_NamingReplicationC.h"

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

class TAO_FT_Naming_Replication_Manager;

/**
 * @class FT_TAO_Storable_Naming_Context
 *
 * @brief This class specializes the TAO_Storable_Naming_Context
 * 'ConcreteImplementor' in the Bridge pattern architecture of the
 * CosNaming::NamingContext implementation.
 *
 */
class TAO_FtNaming_Export TAO_FT_Storable_Naming_Context :
      public TAO_Storable_Naming_Context
{
public:
  /**
   * Constructor that takes in preallocated data structure and takes
   * ownership of it.  Derived class from TAO_Persistent_Naming_Context
   * provides specialization of the resolve operation to support
   * load balancing.
   */
  TAO_FT_Storable_Naming_Context (CORBA::ORB_ptr orb,
                                  PortableServer::POA_ptr poa,
                                  const char *poa_id,
                                  TAO_Storable_Naming_Context_Factory *cxt_factory,
                                  TAO::Storable_Factory *factory);

  /// Destructor.
  virtual ~TAO_FT_Storable_Naming_Context () = default;

  /**
   * Override the resolve operation to support load balancing using
   * the object group manager and associated strategy.
   */
  virtual CORBA::Object_ptr resolve (const CosNaming::Name &n);

  // Set the Naming Manager as a static so that it is available for all
  // naming context implementations.
  static void set_naming_manager (TAO_FT_Naming_Manager *mgr_impl);

  bool is_object_group (const CORBA::Object_ptr obj) const;

 /**
  * Queues a request to update the peer replica instance of this
  * context.
  */
  void propagate_update_notification (FT_Naming::ChangeType change_type);

 /**
  * Find the indicated context below this context.  Returns 0
  * if it cannot be found.
  */
  TAO_FT_Storable_Naming_Context* find_relative_context (
                              const CosNaming::Name &n);

  /**
   * Mark the implementation as stale for replicated persistence support.
   */
  virtual void stale (bool is_stale);

  virtual bool stale (void);

  /**
   * An internal utility used to signal that this context was updated.
   * Check the last_changed_ attribute for the time of the write.
   */
  void context_written (void);

  /**
   * An internal callback invoked by the File_Open_Lock_and_Check
   * object to determine if this context is obsolete with respect to the
   * file object .
   */
  virtual bool is_obsolete (time_t stored_time);

protected:

  static TAO_FT_Naming_Manager *naming_manager_;
  bool stale_;
  TAO_FT_Naming_Replication_Manager *replicator_;

};

TAO_END_VERSIONED_NAMESPACE_DECL

#include /**/ "ace/post.h"
#endif /* TAO_FT_STORABLE_NAMING_CONTEXT_H */
