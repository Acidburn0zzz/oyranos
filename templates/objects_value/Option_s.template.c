{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_sentinel.h"
#include "oyranos_elektra.h"
#include "oyObserver_s.h"
#include "oyBlob_s_.h"
{% endblock %}
