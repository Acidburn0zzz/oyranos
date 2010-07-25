{% include "source_file_header.txt" %}

#ifndef OYRANOS_OBJECT_H
#define OYRANOS_OBJECT_H

{% include "cpp_begin.h" %}

#include <oyranos.h>

/* Function Pointers Definitions Start */
typedef oyPointer (*oyPointer_copy_f )   ( oyPointer, size_t );
typedef int       (*oyPointer_release_f )( oyPointer * );
typedef void      (*oyLockRelease_f )( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyLock_f )       ( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyUnLock_f )     ( oyPointer           look,
                                       const char        * marker,
                                       int                 line );
/* Function Pointers Definitions Stop */

/* Declare oyObject_s Globally */
typedef struct oyObject_s_* oyObject_s;

/**
 *  @brief   Oyranos structure type
 *  @ingroup objects_generic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/22 (Oyranos: 0.1.x)
 *  @date    2008/07/31
 */
typedef enum {
  oyOBJECT_NONE,
  oyOBJECT_NAMED_COLOURS_S,
{% for class in classes %}
  {{ class.enumName }},   /**< {{ class.name }} */
{% endfor %}
  oyOBJECT_MAX
} oyOBJECT_e;

/** @enum    oyNAME_e
 *  @brief   describe the base types of a oyObject_s name
 *  @ingroup objects_generic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2007/10/00
 */
typedef enum {
  oyNAME_NAME,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_NICK,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_DESCRIPTION                   /**< compatible to oyName_s/oyObject_s */
} oyNAME_e;

/** @brief   select from filter
 *  @ingroup objects_value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/27 (Oyranos: 0.1.10)
 *  @date    2009/07/27
 */
#define OY_SELECT_FILTER         2048
/** @brief   select from policy
 *  @ingroup objects_value
 *
 *  Select typical from a associatable oyCMMapi9_s type of filter for providing
 *  common options.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/27 (Oyranos: 0.1.10)
 *  @date    2009/07/27
 */
#define OY_SELECT_COMMON         4096
/** @enum    oyOPTIONATTRIBUTE_e
 *  @brief   usage type
 *  @ingroup objects_value
 *
 *  The types of Oyranos default settings to include into the options set.
 *  So proofing makes no sense for toolkit widgets, but for advanced
 *  graphics displaying. All other Oyranos behaviour settings should be taken
 *  over untouched.
 *
 *  The ".invisible" attribute says a option shall not be displayed in a GUI.
 *  The ".readonly" attribute alllows for unchangeable, static options. They
 *  might be used for passing additional informations.
 *  The ".font" and ".advanced" attributes are described for the according
 *  enum values.
 *
 *  See as well @ref registration
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/10/08 (Oyranos: 0.1.8)
 *  @date    2008/11/27
 */
typedef enum {
  /** basic settings, as typical for toolkits and office/web applications,
   *  e.g. disable proofing or disable mark out of gamut colours */
  oyOPTIONATTRIBUTE_BASIC = 0,
  /** user modified, e.g. after oyOption_SetFromText() */
  oyOPTIONATTRIBUTE_EDIT = 32,
  /** automatically modified, e.g. by a CMM */
  oyOPTIONATTRIBUTE_AUTOMATIC = 64,
  /** advanced settings, as typical for editing, e.g. include proofing
   *  (options appended with ".advanced") */
  oyOPTIONATTRIBUTE_ADVANCED = 128,
  /** front end options, handled by the framework or policy module,
   *  e.g. ".front" */
  oyOPTIONATTRIBUTE_FRONT = 256,
  /** tell this options is included twice*/
  oyOPTIONATTRIBUTE_DOUBLE = 512
} oyOPTIONATTRIBUTE_e;

{% include "cpp_end.h" %}

#endif /* OYRANOS_OBJECT_H */
