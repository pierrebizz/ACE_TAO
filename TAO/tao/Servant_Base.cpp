// $Id$

#include "tao/Servant_Base.h"
#include "tao/Timeprobe.h"
#include "tao/Operation_Table.h"
#include "tao/ORB_Core.h"
#include "tao/POA.h"
#include "tao/Stub.h"
#include "tao/Environment.h"
#include "tao/Server_Request.h"

ACE_RCSID(tao, Servant_Base, "$Id$")

#if !defined (__ACE_INLINE__)
# include "tao/Servant_Base.i"
#endif /* ! __ACE_INLINE__ */

#if defined (ACE_ENABLE_TIMEPROBES)

static const char *TAO_Servant_Base_Timeprobe_Description[] =
{
  "Servant_Base::_find - start",
  "Servant_Base::_find - end"
};

enum
{
  TAO_SERVANT_BASE_FIND_START = 700,
  TAO_SERVANT_BASE_FIND_END
};

// Setup Timeprobes
ACE_TIMEPROBE_EVENT_DESCRIPTIONS (TAO_Servant_Base_Timeprobe_Description,
                                  TAO_SERVANT_BASE_FIND_START);

#endif /* ACE_ENABLE_TIMEPROBES */

TAO_ServantBase::TAO_ServantBase (void)
  : optable_ (0),
    single_threaded_poa_lock_ (0),
    single_threaded_poa_lock_count_ (0)
{
}

TAO_ServantBase::TAO_ServantBase (const TAO_ServantBase &rhs)
  : optable_ (rhs.optable_),
    single_threaded_poa_lock_ (0),
    single_threaded_poa_lock_count_ (0)
{
}

TAO_ServantBase &
TAO_ServantBase::operator= (const TAO_ServantBase &rhs)
{
  this->optable_ = rhs.optable_;
  return *this;
}

TAO_ServantBase::~TAO_ServantBase (void)
{
}

PortableServer::POA_ptr
TAO_ServantBase::_default_POA (CORBA::Environment &ACE_TRY_ENV)
{
  return TAO_ORB_Core_instance ()->root_poa_reference (ACE_TRY_ENV);
}

void
TAO_ServantBase::_add_ref (CORBA::Environment &)
{
}

void
TAO_ServantBase::_remove_ref (CORBA::Environment &)
{
}

CORBA::Boolean
TAO_ServantBase::_is_a (const char* logical_type_id,
                        CORBA::Environment &ACE_TRY_ENV)
{
  const char *id = CORBA::_tc_Object->id (ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  if (ACE_OS::strcmp (logical_type_id, id) == 0)
    {
      return 1;
    }

  return 0;
}

CORBA::Boolean
TAO_ServantBase::_non_existent (CORBA::Environment &)
{
  return 0;
}

int
TAO_ServantBase::_find (const char *opname,
                        TAO_Skeleton& skelfunc,
                        const unsigned int length)
{
  ACE_FUNCTION_TIMEPROBE (TAO_SERVANT_BASE_FIND_START);
  return optable_->find (opname, skelfunc, length);
}

int
TAO_ServantBase::_bind (const char *opname,
                        const TAO_Skeleton skel_ptr)
{
  return optable_->bind (opname, skel_ptr);
}

TAO_Stub *
TAO_ServantBase::_create_stub (CORBA_Environment &ACE_TRY_ENV)
{
  TAO_Stub *stub = 0;

  TAO_POA_Current_Impl *poa_current_impl =
    TAO_ORB_CORE_TSS_RESOURCES::instance ()->poa_current_impl_;

  if (poa_current_impl != 0 &&
      this == poa_current_impl->servant ())
    {
      stub = poa_current_impl->orb_core ().orb ()->create_stub_object (poa_current_impl->object_key (),
                                                                       this->_interface_repository_id (),
                                                                       ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);
    }
  else
    {
      PortableServer::POA_var poa = this->_default_POA (ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);

      CORBA::Object_var object = poa->servant_to_reference (this, ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);

      // Get the stub object
      stub = object->_stubobj ();

      // Increment the reference count since <object> will zap its
      // stub object on deletion.
      stub->_incr_refcnt ();
    }

  return stub;
}

ACE_SYNCH_MUTEX &
TAO_ServantBase::_single_threaded_poa_lock (void)
{
  return *this->single_threaded_poa_lock_;
}

void
TAO_ServantBase::_increment_single_threaded_poa_lock_count (void)
{
  // Only one thread at a time through this code (guarantee provided
  // by the POA).
  u_long current_count = this->single_threaded_poa_lock_count_++;
  if (current_count == 0)
    {
      ACE_NEW (this->single_threaded_poa_lock_,
               ACE_SYNCH_MUTEX);
    }
}

void
TAO_ServantBase::_decrement_single_threaded_poa_lock_count (void)
{
  // Only one thread at a time through this code (guarantee provided
  // by the POA).
  u_long current_count = --this->single_threaded_poa_lock_count_;
  if (current_count == 0)
    {
      delete this->single_threaded_poa_lock_;
      this->single_threaded_poa_lock_ = 0;
    }
}

CORBA::Object_ptr
TAO_ServantBase::_create_collocated_objref (CORBA::ULong type,
                                            CORBA::Environment &ACE_TRY_ENV)
{
  return CORBA::Object::_nil ();
}

TAO_RefCountServantBase::~TAO_RefCountServantBase (void)
{
}

void
TAO_RefCountServantBase::_add_ref (CORBA::Environment &)
{
  ++this->ref_count_;
}

void
TAO_RefCountServantBase::_remove_ref (CORBA::Environment &)
{
  CORBA::ULong new_count = --this->ref_count_;
  if (new_count == 0)
    {
      delete this;
    }
}

TAO_RefCountServantBase::TAO_RefCountServantBase (void)
  : ref_count_ (1)
{
}

TAO_RefCountServantBase::TAO_RefCountServantBase (const TAO_RefCountServantBase &)
  : ref_count_ (1)
{
}

TAO_RefCountServantBase &
TAO_RefCountServantBase::operator= (const TAO_RefCountServantBase &)
{
  return *this;
}

TAO_ServantBase_var::TAO_ServantBase_var (void)
  : ptr_ (0)
{
}

TAO_ServantBase_var::TAO_ServantBase_var (TAO_ServantBase *p)
  : ptr_ (p)
{
}

TAO_ServantBase_var::TAO_ServantBase_var (const TAO_ServantBase_var &b)
  : ptr_ (b.ptr_)
{
  if (this->ptr_ != 0)
    this->ptr_->_add_ref ();
}

TAO_ServantBase_var::~TAO_ServantBase_var (void)
{
  if (this->ptr_ != 0)
    this->ptr_->_remove_ref ();
}

TAO_ServantBase_var &
TAO_ServantBase_var::operator= (TAO_ServantBase *p)
{
  if (this->ptr_ != 0)
    this->ptr_->_remove_ref ();

  this->ptr_ = p;

  return *this;
}

TAO_ServantBase_var &
TAO_ServantBase_var::operator= (const TAO_ServantBase_var &b)
{
  if (this->ptr_ != b.ptr_)
  {
    if (this->ptr_ != 0)
      this->ptr_->_remove_ref ();

    if ((this->ptr_ = b.ptr_) != 0)
      this->ptr_->_add_ref ();
  }

  return *this;
}

TAO_ServantBase *
TAO_ServantBase_var::operator->() const
{
  return this->ptr_;
}

TAO_ServantBase *
TAO_ServantBase_var::in (void) const
{
  return this->ptr_;
}

TAO_ServantBase *&
TAO_ServantBase_var::inout (void)
{
  return this->ptr_;
}

TAO_ServantBase *&
TAO_ServantBase_var::out (void)
{
  if (this->ptr_ != 0)
    this->ptr_->_remove_ref();

  this->ptr_ = 0;

  return this->ptr_;
}

TAO_ServantBase *
TAO_ServantBase_var::_retn (void)
{
  TAO_ServantBase *retval = this->ptr_;
  this->ptr_ = 0;
  return retval;
}

TAO_Stub *
TAO_Local_ServantBase::_create_stub (CORBA_Environment &ACE_TRY_ENV)
{
  PortableServer::ObjectId_var invalid_oid =
    PortableServer::string_to_ObjectId ("invalid");

  TAO_ObjectKey tmp_key (invalid_oid->length (),
                         invalid_oid->length (),
                         invalid_oid->get_buffer (),
                         0);

  // It is ok to use TAO_ORB_Core_instance here since the locality
  // constrained servant does not really register with a POA or get
  // exported remotely.
  //
  // The correct thing to do is to probably use ORB of the default
  // POA. The unfortunate part is that calling default_POA() requires
  // the creation of a local stub, hence causing a infinite loop.
  return TAO_ORB_Core_instance ()->orb ()->create_stub_object (tmp_key,
                                                               this->_interface_repository_id (),
                                                               ACE_TRY_ENV);
}

void
TAO_Local_ServantBase::_dispatch (CORBA::ServerRequest &request,
                                  void *context,
                                  CORBA_Environment &ACE_TRY_ENV)
{
  ACE_THROW (CORBA::BAD_OPERATION ());
}

#if !defined (TAO_HAS_MINIMUM_CORBA)

CORBA::Object_ptr
TAO_DynamicImplementation::_this (CORBA::Environment &ACE_TRY_ENV)
{
  // The _this() function returns a CORBA::Object_ptr for the target
  // object. Unlike _this() for static skeletons, its return type is
  // not interface-specific because a DSI servant may very well
  // incarnate multiple CORBA objects of different types.
  TAO_Stub *stub = this->_create_stub (ACE_TRY_ENV);
  ACE_CHECK_RETURN (CORBA::Object::_nil ());

  // Create a object
  return new CORBA::Object (stub, this, 1);
}

const char *
TAO_DynamicImplementation::_interface_repository_id (void) const
{
  // This should never be called
  return 0;
}

void *
TAO_DynamicImplementation::_downcast (const char *repository_id)
{
  ACE_UNUSED_ARG (repository_id);

  // Don't know enough to do better
  return this;
}

TAO_Stub *
TAO_DynamicImplementation::_create_stub (CORBA::Environment &ACE_TRY_ENV)
{
  // If DynamicImplementation::_this() is invoked outside of the
  // context of a request invocation on a target object being served
  // by the DSI servant, it raises the PortableServer::WrongPolicy
  // exception.
  TAO_POA_Current_Impl *poa_current_impl =
    TAO_ORB_CORE_TSS_RESOURCES::instance ()->poa_current_impl_;

  if (poa_current_impl != 0 &&
      this == poa_current_impl->servant ())
    {
      ACE_THROW_RETURN (PortableServer::POA::WrongPolicy (),
                        0);
    }

  PortableServer::POA_var poa = poa_current_impl->get_POA (ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  CORBA::RepositoryId interface = this->_primary_interface (poa_current_impl->object_id (),
                                                            poa.in (),
                                                            ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  return poa_current_impl->POA_impl ()->orb_core ().orb ()->create_stub_object (poa_current_impl->object_key (),
                                                                                interface,
                                                                                ACE_TRY_ENV);
}

void
TAO_DynamicImplementation::_dispatch (CORBA::ServerRequest &request,
                                      void *context,
                                      CORBA::Environment &ACE_TRY_ENV)
{
  ACE_UNUSED_ARG (context);

  // Delegate to user
  this->invoke (&request, ACE_TRY_ENV);
  ACE_CHECK;

  if (request.response_expected ())
    {
      request.init_reply (ACE_TRY_ENV);
      ACE_CHECK;

      request.dsi_marshal (ACE_TRY_ENV);
      ACE_CHECK;
    }
}

#endif /* TAO_HAS_MINIMUM_CORBA */
