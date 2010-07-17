{% include "source_file_header.txt" %}

#include "oyranos_object_internal.h"

#include "{{ class.name }}.h"
#include "{{ class.privName }}.h"

#include "oyObject_s.h"
#include "oyOptions_s.h"


/* {{ class.name }} common object functions { */
/** Function oyObserver_New
 *  @memberof oyObserver_s
 *  @brief   allocate a new Observer object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI oyObserver_s * OYEXPORT
           oyObserver_New            ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_OBSERVER_S;
# define STRUCT_TYPE oyObserver_s
  int error = 0;
  STRUCT_TYPE * s = 0;

  s = (STRUCT_TYPE*)oyAllocateFunc_(sizeof(STRUCT_TYPE));

  if(!s)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );
  if(error)
    WARNc_S("Problem with memset.");

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyObserver_Copy;
  s->release = (oyStruct_Release_f) oyObserver_Release;
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/** Function oyObserver_Copy
 *  @memberof oyObserver_s
 *  @brief   copy a Observer object
 *
 *  A reference is not possible as no reference counter is available.
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI oyObserver_s * OYEXPORT
           oyObserver_Copy           ( oyObserver_s      * obj,
                                       oyObject_s          object )
{
  oyObserver_s * s = obj;

  oyCheckType__m( oyOBJECT_OBSERVER_S, return 0 )

  s = oyObserver_Copy_( obj, object );

  return s;
}
 
/** Function oyObserver_Release
 *  @memberof oyObserver_s
 *  @brief   release and possibly deallocate a Observer object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI int  OYEXPORT
           oyObserver_Release        ( oyObserver_s     ** obj )
{
  /* ---- start of common object destructor ----- */
  oyObserver_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_OBSERVER_S, return 1 )

  *obj = 0;

  /* ---- end of common object destructor ------- */
  if(s->observer)
    s->observer->release( &s->observer ); s->observer = 0;
  if(s->model)
    s->model->release( &s->model ); s->model = 0;
  if(s->user_data)
    s->user_data->release( &s->user_data ); s->user_data = 0;

  {
    oyDeAlloc_f deallocateFunc = oyDeAllocateFunc_;

    deallocateFunc( s );
  }

  return 0;
}
/* } {{ class.name }} common object functions */

/* {{ class.name }} object specific functions { */
/** Function oyObserver_SignalSend
 *  @memberof oyObserver_s
 *  @brief   send a signal to a Observer object
 *
 *  @param[in]     observer            observer
 *  @param[in]     signal_type         basic signal information
 *  @param[in]     signal_data         advanced informations
 *  @return                            0 - not matching; 1 - match, skip others
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT
           oyObserver_SignalSend     ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  oyObserver_s * s = observer;
  int result = 0;

  oyCheckType__m( oyOBJECT_OBSERVER_S, return 0 )

     /* global signal disabling */
  if(!oyToSignalBlock_m( oyObserverFlagsGet() ) &&
     /* local signal disabling */
     !observer->disable_ref )
    result = observer->signal( observer, signal_type, signal_data );

  return result;
}

/** Function oyStruct_ObserverAdd
 *  @memberof oyObserver_s
 *  @brief   send a signal to a Observer object
 *
 *  @param[in]     model               the to be observed model
 *  @param[in]     observer            the in observation intereressted object
 *  @param[in]     user_data           additional static informations
 *  @param[in]     signalFunc          the signal handler;
 *                                     defaults to oyStructSignalForward_
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI int  OYEXPORT
           oyStruct_ObserverAdd      ( oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc )
{
  oyObserver_s * s = 0,
               * obs = 0;
  int error = !model || !observer;
  oyStructList_s * list = 0;
  int n,i, found;

  if(!signalFunc)
    signalFunc = oyStructSignalForward_;

  /* add to model */
  list = oyStruct_ObserverListGet_( model, OY_SIGNAL_OBSERVERS );
  if(!error && list)
  {
    found = 0;
    n = oyStructList_Count( list );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s*) oyStructList_GetType_( list,
                                                   i, oyOBJECT_OBSERVER_S );
      if(observer == obs->observer && obs->signal == signalFunc)
        ++found;
    }

    /* add new oyObserver_s */
    if(found == 0)
    {
      s = oyObserver_New( 0 );
      if(observer)
        s->observer = observer->copy( observer, 0 );
      s->model = model->copy( model, 0 );
      if(user_data)
        s->user_data = user_data->copy( user_data, 0 );
      s->signal = signalFunc;

      oyStructList_MoveIn( list, (oyStruct_s**)&s, -1, 0 );
    }
  }

  /* add to observer */
  if(!error)
    list = oyStruct_ObserverListGet_( observer, OY_SIGNAL_MODELS );
  if(!error && list)
  {
    found = 0;
    n = oyStructList_Count( list );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s*) oyStructList_GetType_( list,
                                                   i, oyOBJECT_OBSERVER_S );
      if(model == obs->model && obs->signal == signalFunc)
        ++found;
    }

    /* add oyObserver_s */
    if(found == 0 && !s)
    {
      s = oyObserver_New( 0 );
      if(observer)
        s->observer = observer->copy( observer, 0 );
      s->model = model->copy( model, 0 );
      if(user_data)
        s->user_data = user_data->copy( user_data, 0 );
      s->signal = signalFunc;
    }

    if(s)
      oyStructList_MoveIn( list, (oyStruct_s**)&s, -1, 0 );
  }

  return error;
}

/** Function oyStruct_ObserverRemove
 *  @memberof oyObserver_s
 *  @brief   disconnect a observer from a object
 *
 *  @param[in,out] observer            the model
 *  @param[in]     model               the pattern
 *  @param[in]     signalFunc          the signal handler to remove
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT
           oyStruct_ObserverRemove   ( oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyObserver_Signal_f signalFunc )
{
  int error = !model || !observer;
  oyStructList_s * list = 0;

  if(!error)
  {
    list = oyStruct_ObserverListGet_( model, OY_SIGNAL_OBSERVERS );
    error = oyStruct_ObserverRemove_( list, observer, 1, signalFunc );
  }
  if(!error)
  {
    list = oyStruct_ObserverListGet_( observer, OY_SIGNAL_MODELS );
    error = oyStruct_ObserverRemove_( list, model, 0, signalFunc );
  }

  return error;
}

/** Function oyStruct_ObserverSignal
 *  @memberof oyObserver_s
 *  @brief   send a signal to all ovservers of a model
 *
 *  @param[in]     model               the model
 *  @param[in]     signal_type         the basic signal type to emit
 *  @param[in,out] signal_data         the advanced signal information
 *  @return                            0 - no handler found; 1 - handler found;
 *                                     < 0 error or issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/27 (Oyranos: 0.1.10)
 *  @date    2009/10/27
 */
OYAPI int  OYEXPORT
           oyStruct_ObserverSignal   ( oyStruct_s        * model,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  oyObserver_s * obs = 0;
  int error = !model, t_err = 0;
  oyOption_s * o = 0;
  oyStructList_s * observers = 0;
  int n,i, result = 0;

  if(oyToSignalBlock_m( oyObserverFlagsGet() ))
    return 0;

  if(!error)
    o = oyOptions_Find( model->oy_->handles_,
                        OY_SIGNAL_OBSERVERS );

  if(!o)
    return 0;

  if(!error)
  {
    observers = (oyStructList_s*)oyOption_StructGet( o, oyOBJECT_STRUCT_LIST_S);
    n = oyStructList_Count( observers );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s*) oyStructList_GetType_( observers,
                                                   i, oyOBJECT_OBSERVER_S );
      if(obs)
      {
        if(obs->model == model)
        {
          if(oy_debug_signals)
          {
            WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]", _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
          }
          t_err = oyObserver_SignalSend( obs, signal_type, signal_data );
          ++result;
        }
        else
        {
          WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]",_("found observer of wrong type"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
      } else
        WARNc3_S( "\n\t%s: oyObservers_s[%s]",_("found no observer"),
                    oySignalToString(signal_type),
                    oyStruct_TypeToText((oyStruct_s*)observers) );
    }
  }

  if(result)
    return result;
  return error;
}

/** Function oyStruct_DisableSignalSend
 *  @memberof oyObserver_s
 *  @brief   disable sending a signal to all ovservers of a model
 *
 *  @param[in]     model               the model
 *  @return                            0 - no handler found; 1 - error
 *                                     < 0 error or issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/04/07 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT
           oyStruct_DisableSignalSend( oyStruct_s        * model )
{
  oyObserver_s * obs = 0;
  int error = !model;
  oyOption_s * o = 0;
  oyStructList_s * observers = 0;
  int n,i;

  if(!error)
    o = oyOptions_Find( model->oy_->handles_, OY_SIGNAL_OBSERVERS );

  if(!o)
    return 0;

  if(!error)
  {
    observers = (oyStructList_s*)oyOption_StructGet( o, oyOBJECT_STRUCT_LIST_S );
    n = oyStructList_Count( observers );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s*) oyStructList_GetType_( observers,
                                                   i, oyOBJECT_OBSERVER_S );
      if(obs)
        ++obs->disable_ref;
    }
  }

  return error;
}

/** Function oyStruct_EnableSignalSend
 *  @memberof oyObserver_s
 *  @brief   reenable sending a signal to all ovservers of a model
 *
 *  @param[in]     model               the model
 *  @return                            0 - no handler found; 1 - error
 *                                     < 0 error or issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/04/07 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT
           oyStruct_EnableSignalSend ( oyStruct_s        * model )
{
  oyObserver_s * obs = 0;
  int error = !model;
  oyOption_s * o = 0;
  oyStructList_s * observers = 0;
  int n,i;

  if(!error)
    o = oyOptions_Find( model->oy_->handles_, OY_SIGNAL_OBSERVERS );

  if(!o)
    return 0;

  if(!error)
  {
    observers = (oyStructList_s*)oyOption_StructGet( o, oyOBJECT_STRUCT_LIST_S );

    n = oyStructList_Count( observers );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s*) oyStructList_GetType_( observers,
                                                   i, oyOBJECT_OBSERVER_S );
      if(obs)
        --obs->disable_ref;
      if(obs->disable_ref < 0)
      {
        obs->disable_ref = 0;
        WARNcc3_S( model, "%s: %s[%d]", _("Already enabled"),
                   oyStruct_GetText( (oyStruct_s*)obs, oyNAME_NAME, 1),
                   oyObject_GetId( o->value->oy_struct->oy_ ) );
      }
    }
  }

  return error;
}

/** Function oyStruct_ObserversCopy
 *  @memberof oyObserver_s
 *  @brief   copy a objects observers and models to a different object
 *
 *  oyStruct_ObserversCopy is a convenience function for
 *  oyStruct_ObserverCopyModel() and oyStruct_ObserverCopyObserver().
 *
 *  @param[in,out] object              the object
 *  @param[in]     pattern             the pattern to copy from
 *  @param[in]     flags               0x02 omit observers; 0x04 omit models
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT
           oyStruct_ObserversCopy    ( oyStruct_s        * object,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags )
{
  int error = 0;

  if(!error && !(flags & 0x04))
    error = oyStruct_ObserverCopyModel(object, pattern, flags);
  if(!error && !(flags & 0x02))
    error = oyStruct_ObserverCopyObserver(object, pattern, flags);

  return error;
}

/** Function oyStruct_ObserverCopyModel
 *  @memberof oyObserver_s
 *  @brief   copy a models observers to a different model
 *
 *  The observers are copied from pattern with object type to a model with the
 *  same object type.
 *
 *  @param[in,out] model               the model
 *  @param[in]     pattern             the pattern to copy from
 *  @param[in]     flags               0x01 omit observer idendity check
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT
           oyStruct_ObserverCopyModel( oyStruct_s        * model,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags )
{
  oyObserver_s * obs = 0;
  int error = !model;
  oyStructList_s * observers = 0;
  int n,i;

  if( !model || !pattern )
    return 0;

  observers = oyStruct_ObserverListGet_( pattern, OY_SIGNAL_OBSERVERS );

  if(!error)
  {
    n = oyStructList_Count( observers );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s*) oyStructList_GetType_( observers,
                                                   i, oyOBJECT_OBSERVER_S );
      if(obs &&
         (!(flags & 0x01) || obs->model == pattern))
      { 
        if(oy_debug_signals)
        {
          WARNc5_S( "\n\tCopy %s: %s[%d]->%s[%d]", _("Signal"),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
        error = oyStruct_ObserverAdd( (oyStruct_s*)model, obs->observer,
                                obs->user_data, obs->signal );

      }
      else
      {
        if(obs)
        {
          WARNc5_S( "\n\tCopy: %s: %s[%d]->%s[%d]",
                    _("found observer of wrong type"),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
        else
        {
          WARNc2_S( "\n\toyObservers_s %s ",_("found no observer"),
                    oyStruct_TypeToText((oyStruct_s*)observers) );
        }
      }
    }
  }

  return error;

}

/** Function oyStruct_ObserverCopyObserver
 *  @memberof oyObserver_s
 *  @brief   copy a observers models to a different observer
 *
 *  The models are copied from pattern with object type to a object with the
 *  same object type.
 *
 *  @param[in,out] observer            the observer
 *  @param[in]     pattern             the pattern to copy from
 *  @param[in]     flags               0x01 omit model idendity check
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT
           oyStruct_ObserverCopyObserver (
                                       oyStruct_s        * observer,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags )
{
  oyObserver_s * obs = 0;
  int error = !observer;
  oyStructList_s * list = 0;
  int n,i;

  if( !observer || !pattern )
    return 0;

  list = oyStruct_ObserverListGet_( pattern, OY_SIGNAL_MODELS );

  if(!error)
  {
    n = oyStructList_Count( list );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s*) oyStructList_GetType_( list,
                                                   i, oyOBJECT_OBSERVER_S );
      if(obs &&
         (!(flags & 0x01) || obs->observer == pattern))
      { 
        if(oy_debug_signals)
        {
          WARNc5_S( "\n\tCopy %s: %s[%d]->%s[%d]", _("Signal"),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
        error = oyStruct_ObserverAdd( obs->model, observer,
                                obs->user_data, obs->signal );

      }
      else
      {
        if(obs)
        {
          WARNc5_S( "\n\tCopy: %s: %s[%d]->%s[%d]",
                    _("found observer of wrong type"),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
        else
        {
          WARNc2_S( "\n\t%s ",_("found no observer"),
                    oyStruct_TypeToText( (oyStruct_s*)list) );
        }
      }
    }
  }

  return error;

}

/**
 *  Function oyStruct_IsObserved
 *  @memberof oyObserver_s
 *  @brief   return object observation status
 *
 *  Check if a object is observed by others.
 *
 *  @param         model               model to ask
 *  @param         observer            which observes that model
 *  @return                            true or false
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/10/28
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 */
OYAPI int  OYEXPORT
           oyStruct_IsObserved       ( oyStruct_s        * model,
                                       oyStruct_s        * observer )
{
  int observed = 0;
  int i,n = 0;
  oyOption_s * o = 0;
  int error = 0;

  if(model->oy_->handles_)
    n = oyStructList_Count( (model->oy_->handles_)->list );
  for(i = 0; i < n; ++i)
  {
    o = (oyOption_s*) oyStructList_Get_(
                              (model->oy_->handles_)->list, i );
    if( oyStrcmp_( o->registration, OY_SIGNAL_OBSERVERS ) == 0)
    {
      if(observer)
      {
        oyStructList_s * observers = 0;
        int j_n,j;

        observers = (oyStructList_s*)oyOption_StructGet( o,
                                                       oyOBJECT_STRUCT_LIST_S );

        if(!error)
        {
          j_n = oyStructList_Count( observers );
          for(j = 0; j < j_n; ++j)
          {
            oyObserver_s * obs;
            obs = (oyObserver_s*) oyStructList_GetType_( observers,
                                                   j, oyOBJECT_OBSERVER_S );
            if(obs && obs->observer == observer)
            { 
              observed = 1;
              break;
            }
          }
        }
        
      } else
      {
        observed = 1;
        break;
      }
    }
  }

  return observed;
}

uint32_t   oy_observer_flags = 0;
/** Function oyObserverFlagsGet
 *  @memberof oyObserver_s
 *  @brief   get global flags for Observation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI uint32_t OYEXPORT
           oyObserverFlagsGet        ( void )
{
  return oy_observer_flags;
}

/** Function oyObserverFlagsSet
 *  @memberof oyObserver_s
 *  @brief   set global flags for Observation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI int  OYEXPORT
           oyObserverFlagsSet        ( uint32_t            flags )
{ 
  oy_observer_flags = flags;
  return 0;
}
/* } {{ class.name }} object specific functions */
