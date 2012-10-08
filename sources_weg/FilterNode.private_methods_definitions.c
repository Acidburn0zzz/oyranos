#include "oyranos_conversion_internal.h"

/** Function  oyFilterNode_AddToAdjacencyLst_
 *  @memberof oyFilterNode_s
 *  @brief    Get a graphs adjazency list
 *  @internal
 *
 *  @param[in]     s                   the start node
 *  @param[in,out] nodes               the collected nodes
 *  @param[in,out] edges               the collected edges
 *  @param[in]     mark                a search string to get sub graphs
 *  @param[in]     flags               - OY_INPUT omit input direction
 *                                     - OY_OUTPUT omit output direction
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int  oyFilterNode_AddToAdjacencyLst_ ( oyFilterNode_s_    * s,
                                       oyFilterNodes_s_   * nodes,
                                       oyFilterPlugs_s_   * edges,
                                       const char         * mark,
                                       int                  flags )
{
  int n, i, j, p_n;
  oyFilterPlug_s_ * p = 0;

  /* Scan the input/plug side for unknown nodes, add these and continue in
   * the direction of previous unknown edges...
   */
  if(!(flags & OY_INPUT))
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 1, 0 );
    for( i = 0; i < n; ++i )
    {
      if( s->plugs[i] && ((oyFilterPlug_s_*)s->plugs[i])->remote_socket_ )
        if(oyAdjacencyListAdd_( (oyFilterPlug_s*)s->plugs[i], (oyFilterNodes_s*)nodes, (oyFilterPlugs_s*)edges, mark, flags ))
          oyFilterNode_AddToAdjacencyLst_( ((oyFilterSocket_s_*)((oyFilterPlug_s_*)s->plugs[i])->remote_socket_)->node,
                                           nodes, edges, mark, flags );
    }
  }

  /* ... same on the output/socket side */
  if(!(flags & OY_OUTPUT))
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 0, 0 );
    for( i = 0; i < n; ++i )
    {
      if( s->sockets[i] && ((oyFilterSocket_s_*)s->sockets[i])->requesting_plugs_ )
      {
        p_n = oyFilterPlugs_Count( ((oyFilterSocket_s_*)s->sockets[i])->requesting_plugs_ );
        for( j = 0; j < p_n; ++j )
        {
          p = (oyFilterPlug_s_*)oyFilterPlugs_Get( ((oyFilterSocket_s_*)s->sockets[i])->requesting_plugs_, j );

          if(oyAdjacencyListAdd_( (oyFilterPlug_s*)p, (oyFilterNodes_s*)nodes, (oyFilterPlugs_s*)edges, mark, flags ))
            oyFilterNode_AddToAdjacencyLst_( (oyFilterNode_s_*)p->node,
                                             nodes, edges, mark, flags );
        }
      }
    }
  }

  return 0;
}

/** Function  oyFilterNode_SetContext_
 *  @memberof oyFilterNode_s
 *  @brief    Set module context in a filter
 *  @internal
 *
 *  The api4 data is passed to a interpolator specific transformer. The result
 *  of this transformer will on request be cached by Oyranos as well.
 *
 *  @param[in]     node                filter
 *  @param[in,out] blob                context to fill
 *  @return                            error
 *
 *  @version Oyranos: 0.5.0
 *  @date    2012/06/12
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 */
int          oyFilterNode_SetContext_( oyFilterNode_s_    * node,
                                       oyBlob_s_          * blob  )
{
  int error = 0;
  oyFilterCore_s_ * core_ = node->core;

  if(error <= 0)
  {
          size_t size = 0;
          oyHash_s * hash = 0,
                   * hash_out = 0;
          const char * hash_text_ = 0;
          char * hash_text = 0,
               * hash_temp = 0;
          int hash_text_len;
          oyPointer ptr = 0;
          oyPointer_s * cmm_ptr = 0,
                     * cmm_ptr_out = 0;


          /*  Cache Search
           *  1.     hash from input
           *  2.     query for hash in cache
           *  3.     check
           *  3a.       eighter take cache entry
           *  3b.       or ask CMM
           *  3b.1.                update cache entry
           */

          /* 1. create hash text */
          if(core_->api4_->oyCMMFilterNode_GetText)
          {
            hash_temp = core_->api4_->oyCMMFilterNode_GetText(
                                             (oyFilterNode_s*)node, oyNAME_NICK,
                                                           oyAllocateFunc_ );
            hash_text_ = hash_temp;
          } else
            hash_text_ =oyFilterNode_GetText((oyFilterNode_s*)node,oyNAME_NICK);

          hash_text_len = oyStrlen_( hash_text_ );

          hash_text = oyAllocateFunc_(hash_text_len + 16);
          oySprintf_( hash_text, "%s:%s", node->api7_->context_type,
                                          hash_text_ );

          if(oy_debug == 1)
          {
            size = 0;
            ptr = oyFilterNode_TextToInfo_( node, &size, oyAllocateFunc_ );
            if(ptr)
              oyWriteMemToFile_( "test_dbg_colour.icc", ptr, size );
          }

          /* 2. query in cache for api7 */
          hash_out = oyCMMCacheListGetEntry_( hash_text );

          if(error <= 0)
          {
            /* 3. check and 3.a take*/
            cmm_ptr_out = (oyPointer_s*) oyHash_GetPointer( hash_out,
                                                        oyOBJECT_POINTER_S);

            if(!(cmm_ptr_out && oyPointer_GetPointer(cmm_ptr_out)) || blob)
            {
              oySprintf_( hash_text, "%s:%s", core_->api4_->context_type,
                                              hash_text_ );
              /* 2. query in cache for api4 */
              hash = oyCMMCacheListGetEntry_( hash_text );
              cmm_ptr = (oyPointer_s*) oyHash_GetPointer( hash,
                                                        oyOBJECT_POINTER_S);

              if(!cmm_ptr)
              {
                size = 0;
                cmm_ptr = oyPointer_New(0);
              }

              /* write the context to memory */
              if(blob)
              {

                error = oyOptions_SetFromText( &node->tags, "////verbose",
                                               "true", OY_CREATE_NEW );

                /* oy_debug is used to obtain a complete data set */
                ptr = core_->api4_->oyCMMFilterNode_ContextToMem(
                                                   (oyFilterNode_s*)node, &size,
                                                              oyAllocateFunc_ );
                oyBlob_SetFromData( (oyBlob_s*)blob, ptr, size, core_->api4_->context_type );
                error = oyOptions_SetFromText( &node->tags, "////verbose",
                                               "false", 0 );

                goto clean;
              }

              if(!oyPointer_GetPointer(cmm_ptr))
              {
                /* 3b. ask CMM */
                ptr = core_->api4_->oyCMMFilterNode_ContextToMem(
                                                   (oyFilterNode_s*)node, &size,
                                                              oyAllocateFunc_ );

                if(!ptr || !size)
                {
                  oyMessageFunc_p( oyMSG_ERROR, (oyStruct_s*) node,
                  OY_DBG_FORMAT_ "no device link for caching\n%s", OY_DBG_ARGS_,
                  oyFilterNode_GetText( (oyFilterNode_s*)node, oyNAME_NICK ));
                  error = 1;
                  oyPointer_Release( &cmm_ptr );
                }

                if(!error)
                {
                  error = oyPointer_Set( cmm_ptr, core_->api4_->id_,
                                         core_->api4_->context_type,
                                    ptr, "oyPointerRelease", oyPointerRelease);
                  oyPointer_SetSize( cmm_ptr, size );

                  /* 3b.1. update cache entry */
                  error = oyHash_SetPointer( hash, (oyStruct_s*) cmm_ptr);
                }
              }


              if(error <= 0 && cmm_ptr && oyPointer_GetPointer(cmm_ptr))
              {
                if(node->backend_data && node->backend_data->release)
                node->backend_data->release( (oyStruct_s**)&node->backend_data);

                if( oyStrcmp_( node->api7_->context_type,
                               core_->api4_->context_type ) != 0 )
                {
                  cmm_ptr_out = oyPointer_New(0);
                  error = oyPointer_Set( cmm_ptr_out, node->api7_->id_,
                                         node->api7_->context_type, 0, 0, 0);

                  /* search for a convertor and convert */
                  oyPointer_ConvertData( cmm_ptr, cmm_ptr_out,
                                         (oyFilterNode_s*)node );
                  node->backend_data = cmm_ptr_out;
                  /* 3b.1. update cache entry */
                  error = oyHash_SetPointer( hash_out,
                                              (oyStruct_s*) cmm_ptr_out);

                } else
                  node->backend_data = oyPointer_Copy( cmm_ptr, 0 );
              }

              if(oy_debug == 1)
              {
                int id = oyFilterNode_GetId( (oyFilterNode_s*)node );
                char * file_name = 0;
                oyAllocHelper_m_( file_name, char, 80, 0, return 1 );
                sprintf( file_name, "test_dbg_colour_dl-%d.icc", id );
                if(ptr && size && node->backend_data)
                  oyWriteMemToFile_( file_name, ptr, size );
                oyFree_m_(file_name);
              }

            } else
              node->backend_data = cmm_ptr_out;

          }


    clean:
    if(hash_temp) oyDeAllocateFunc_(hash_temp);
    if(hash_text) oyDeAllocateFunc_(hash_text);
  }

  return error;
}

/** Function  oyFilterNode_GetData_
 *  @memberof oyFilterNode_s
 *  @brief    Get the processing data from a filter node
 *  @internal
 *
 *  @param[in]     node                filter
 *  @param[in]     get_plug            1 get input, 0 get output data
 *  @return                            the data list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/04 (Oyranos: 0.1.8)
 *  @date    2008/11/04
 */
oyStructList_s * oyFilterNode_GetData_(oyFilterNode_s_    * node,
                                       int                  get_plug )
{
  int error = !node;
  oyStructList_s * datas = 0;
  oyStruct_s * data = 0;
  int i, n;

  if(error <= 0)
  {
    datas = oyStructList_New(0);

    if(get_plug)
    {
          /* pick all plug (input) data */
          n = oyFilterNode_EdgeCount( (oyFilterNode_s*)node, 1, 0 );
          for( i = 0; i < n && error <= 0; ++i)
          if(node->plugs[i])
          {
            data = 0;
            if(node->plugs[i]->remote_socket_->data)
              data = node->plugs[i]->remote_socket_->data->copy( node->plugs[i]->remote_socket_->data, 0 );
            else
              data = (oyStruct_s*) oyOption_New(0);
            error = oyStructList_MoveIn( datas, &data, -1, 0 );
            ++i;
          }
    } else
    {
          /* pick all sockets (output) data */
          n = oyFilterNode_EdgeCount( (oyFilterNode_s*)node, 0, 0 );
          for( i = 0; i < n && error <= 0; ++i)
          if(node->sockets[i])
          {
            data = 0;
            if(node->sockets[i]->data)
              data = node->sockets[i]->data->copy( node->sockets[i]->data, 0 );
            else
              data = (oyStruct_s*) oyOption_New(0);
            error = oyStructList_MoveIn( datas, &data, -1, 0 );
            ++i;
          }

    }
  }

  return datas;
}

/** Function  oyFilterNode_GetLastFromLinear_
 *  @memberof oyFilterNode_s
 *  @brief    Get last node from a linear graph
 *  @internal
 *
 *  @param[in]     first               filter
 *  @return                            last node
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/16 (Oyranos: 0.1.8)
 *  @date    2008/07/16
 */
oyFilterNode_s *   oyFilterNode_GetLastFromLinear_ (
                                       oyFilterNode_s_    * first )
{
  oyFilterNode_s * next = 0,
                 * last = 0;

      next = last = (oyFilterNode_s*)first;

      while(next)
      {
        next = oyFilterNode_GetNextFromLinear_( (oyFilterNode_s_*)next );

        if(next)
          last = next;
      }

  return last;
}

/** Function  oyFilterNode_GetNextFromLinear_
 *  @memberof oyFilterNode_s
 *  @brief    Get next node from a linear graph
 *  @internal
 *
 *  @param[in]     first               filter
 *  @return                            next node
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/01 (Oyranos: 0.1.8)
 *  @date    2008/11/01
 */
oyFilterNode_s *   oyFilterNode_GetNextFromLinear_ (
                                       oyFilterNode_s_    * first )
{
  oyFilterNode_s_ * next = 0;
  oyFilterSocket_s_ * socket = 0;
  oyFilterPlug_s * plug = 0;

  {
    socket = first->sockets[0];

    if(socket)
      plug = oyFilterPlugs_Get( socket->requesting_plugs_, 0 );
    if(plug)
      next = ((oyFilterPlug_s_*)plug)->node;
    else
      next = 0;
    oyFilterPlug_Release( &plug );
  }

  return (oyFilterNode_s*)next;
}

/**
 *  @internal
 *  Info profilbody */
char info_profile_data[320] =
  {
/*0*/    0,0,1,64, 'o','y','r','a',
    2,48,0,0, 'n','o','n','e',
    'R','G','B',32, 'L','a','b',32,
    0,0,0,0,0,0,0,0,
/*32*/    0,0,0,0,97,99,115,112,
    '*','n','i','x',0,0,0,0,
    110,111,110,101,110,111,110,101,
    -64,48,11,8,-40,-41,-1,-65,
/*64*/    0,0,0,0,0,0,-10,-42,
    0,1,0,0,0,0,-45,45,
    'o','y','r','a',0,0,0,0,
    0,0,0,0,0,0,0,0,
/*96*/    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
/*128*/    0,0,0,3,'d','e','s','c',
    0,0,0,-88,0,0,0,33,
    'c','p','r','t',0,0,0,-52,
    0,0,0,29,'I','n','f','o',
/*160*/    0,0,0,-20,0,0,0,0,
    't','e','x','t',0,0,0,0,
    'F','i','l','t','e','r',' ','I',
    'n','f','o',' ','X','M','L',0,
/*192*/    0,0,0,0,0,0,0,0,
    0,0,0,0,'t','e','x','t',
    0,0,0,0,110,111,116,32,
    99,111,112,121,114,105,103,104,
/*224*/    116,101,100,32,100,97,116,97,
    0,0,0,0,'t','e','x','t',
    0,0,0,0,'s','t','a','r',
    't',0,0,0,0,0,0,0,
/*256*/    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
  };

/** Function  oyFilterNode_TextToInfo_
 *  @memberof oyFilterNode_s
 *  @brief    Serialise filter node to binary
 *  @internal
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" text tag.
 *  Not useable for binary contexts.
 *
 *  This function is currently a ICC only thing and yet just for debugging
 *  useful.
 *
 *  @param[in,out] node                filter node
 *  @param[out]    size                output size
 *  @param[in]     allocateFunc        memory allocator
 *  @return                            the profile container
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyPointer    oyFilterNode_TextToInfo_( oyFilterNode_s_    * node,
                                       size_t             * size,
                                       oyAlloc_f            allocateFunc )
{
  oyPointer ptr = 0;
  icHeader * header = 0;
  size_t len = 244, text_len = 0;
  char * text = 0;
  const char * temp = 0;
  uint32_t * mem = 0;

  if(!node)
    return 0;

  temp = oyFilterNode_GetText( (oyFilterNode_s*)node, oyNAME_NAME );

  text_len = strlen(temp) + 1;
  len += text_len + 1;
  len = len > 320 ? len : 320;
  ptr = allocateFunc(len);
  header = ptr;

  if(ptr)
  {
    *size = len;
    memset(ptr,0,len);
    memcpy(ptr, info_profile_data, 320);

    text = ((char*)ptr)+244;
    sprintf(text, "%s", temp);
    header->size = oyValueUInt32( len );
    mem = ptr;
    mem[41] = oyValueUInt32( text_len + 8 );
  }

  return ptr;
}

/**
 *  @internal
 *  Function oyFilterNode_Observe_
 *  @memberof oyFilterNode_s
 *  @brief   observe filter options
 *
 *  Implements oyObserver_Signal_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int      oyFilterNode_Observe_       ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  int handled = 0;
  int i,n;
  oyFilterSocket_s * socket = 0;
  oyFilterNode_s_ * node = 0;
  oyObserver_s  * obs = observer;

  if(observer && observer->model &&
     observer->model->type_ == oyOBJECT_OPTIONS_S &&
     observer->observer && observer->observer->type_== oyOBJECT_FILTER_NODE_S)
  {
    if(oy_debug_signals)
      WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]", _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

    node = (oyFilterNode_s_*)observer->observer;

    /* invalidate the context */
    if(node->backend_data)
      node->backend_data->release( (oyStruct_s**)&node->backend_data );

    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)node, 0, 0 );
    for(i = 0; i < n; ++i)
    {
      socket = oyFilterNode_GetSocket( (oyFilterNode_s*)node, i );
      /* forward to the downward graph */
      oyFilterSocket_SignalToGraph( socket, signal_type );
      oyFilterSocket_Release( &socket );
    }
  }

  return handled;
}
