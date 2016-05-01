#pragma once

#ifndef access

#define access(R,C,T) (*(R *) (((unsigned int) C) + T))
#define m_access(R,C,T)	((R *) (((unsigned int) C) + T))
#define p_access(R,C,T) ((R)(((unsigned int)C) + T))

#endif