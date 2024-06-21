#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include MUT_H
#include MLO_H
#include CNS_H
#include SLB_H
#include CGV_H

cgv_interaction *cgv_NetObject(cgv_interaction *head, char *name)
{
  cgv_interaction *ci;
  ci=(cgv_interaction *)mbkalloc(sizeof(cgv_interaction));
  ci->object_type=CGV_NET_TYPE;
  ci->name=namealloc(name);
  ci->NEXT=head;
  return ci;
}

cgv_interaction *cgv_GateObject(cgv_interaction *head, char *name)
{
  cgv_interaction *ci;
  ci=(cgv_interaction *)mbkalloc(sizeof(cgv_interaction));
  ci->object_type=CGV_BOX_TYPE;
  ci->name=namealloc(name);
  ci->NEXT=head;
  return ci;
}

cgv_interaction *cgv_ConnectorObject(cgv_interaction *head, char *name)
{
  cgv_interaction *ci;
  ci=(cgv_interaction *)mbkalloc(sizeof(cgv_interaction));
  ci->object_type=CGV_CON_TYPE;
  ci->name=namealloc(name);
  ci->NEXT=head;
  return ci;
}

void cgv_FreeObjects(cgv_interaction *head)
{
  cgv_interaction *ci;
  while (head!=NULL)
    {
      ci=head->NEXT;
      mbkfree(head);
      head=ci;
    }
}

void cgv_HiLight(cgvfig_list *cgvf, cgv_interaction *itr, int complete, markobject mark)
{
  complete_extract_list(cgvf, itr, complete, mark);
}

cgvfig_list *cgv_Extract(cgvfig_list *cgvf, cgv_interaction *itr, int complete)
{
  complete_extract_list(cgvf, itr, complete, NULL);
  return finish_extract(cgvf);
}
