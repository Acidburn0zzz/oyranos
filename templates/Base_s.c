{% extends "oyStruct_s.template.c" %}
{% load gsoc_extensions %}

{% block LocalIncludeFiles %}
#include "{{ class.name }}_.h"
{% endblock %}

{% block GeneralPublicMethodsDefinitions %}
/** Function oy{{ class.baseName }}_New
 *  @memberof {{ class.name }}
 *  @brief   allocate a new {{ class.baseName }} object
 */
OYAPI {{ class.name }} * OYEXPORT
  oy{{ class.baseName }}_New( oyObject_s object )
{
  oyObject_s s = object;
  {{ class.privName }} * {{ class.baseName|lower }} = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 );

  {{ class.baseName|lower }} = oy{{ class.baseName }}_New_( s );

  return ({{ class.name }}*) {{ class.baseName|lower }};
}

/** Function oy{{ class.baseName }}_Copy
 *  @memberof {{ class.name }}
 *  @brief   copy or reference a {{ class.baseName }} object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     {{ class.baseName|lower }}                 {{ class.baseName }} struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triffers a real copy
 */
OYAPI {{ class.name }}* OYEXPORT
  oy{{ class.baseName }}_Copy( {{ class.name }} *{{ class.baseName|lower }}, oyObject_s object )
{
  {{ class.privName }} * s = ({{ class.privName }}*) {{ class.baseName|lower }};

  if(s)
    oyCheckType__m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 0 );

  s = oy{{ class.baseName }}_Copy_( s, object );

  return ({{ class.name }}*) s;
}
 
/** Function oy{{ class.baseName }}_Release
 *  @memberof {{ class.name }}
 *  @brief   release and possibly deallocate a {{ class.name }} object
 *
 *  @param[in,out] {{ class.baseName|lower }}                 {{ class.baseName }} struct object
 */
OYAPI int OYEXPORT
  oy{{ class.baseName }}_Release( {{ class.name }} **{{ class.baseName|lower }} )
{
  {{ class.privName }} * s = 0;

  if(!{{ class.baseName|lower }} || !*{{ class.baseName|lower }})
    return 0;

  s = ({{ class.privName }}*) *{{ class.baseName|lower }};

  oyCheckType__m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 1 )

  *{{ class.baseName|lower }} = 0;

  return oy{{ class.baseName }}_Release_( &s );
}
{% endblock GeneralPublicMethodsDefinitions %}
