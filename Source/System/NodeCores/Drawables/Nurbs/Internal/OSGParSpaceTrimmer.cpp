/*---------------------------------------------------------------------------*\
 *                           OpenSG NURBS Library                            *
 *                                                                           *
 *                                                                           *
 * Copyright (C) 2001-2004 by the University of Bonn, Computer Graphics Group*
 *                                                                           *
 *                         http://cg.cs.uni-bonn.de/                         *
 *                                                                           *
 * contact: edhellon@cs.uni-bonn.de, guthe@cs.uni-bonn.de, rk@cs.uni-bonn.de *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/
#include "OSGParSpaceTrimmer.h"
#ifdef OSG_ADAPTIVE_QUAD_TREE
#include "OSGQuadTreeCreator.h"
#endif
#include "OSGSimplePolygon.h"

#include "OSGpredicates.h"
#include <float.h>

OSG_USING_NAMESPACE

#ifdef OSG_WIN32_ICL
#pragma warning( disable : 171 )
#endif

#ifdef OSG_WIN32
#pragma warning (disable : 985)
#endif

#ifdef OSG__DEBUG
 #ifdef OSG_WIN32
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
 #endif
#endif

#ifdef OSG_USE_SIMPLIFIER
 #define OSG_SIMPLIFIER_TESS_ERR 0.75
#endif

#define ROUND_EPS	1e-6

#define OSG_SPLIT_HOLE_CELLS

void dumpcontrolpoints( BezierCurve2D & fasz )
{
  vec2dvector &cptemp = fasz.getControlPointVector();
  for (unsigned int kkk = 0; kkk < cptemp.size(); ++kkk)
    std::cerr << " " << cptemp[ kkk ];
  std::cerr << std::endl;

}

void plotbezier( BezierCurve2D & fasz, std::ofstream & out )
{

  for( double t = 0.0; t <= 1.0; t += 0.001 ) {
    int err;
    vec2d p = fasz.computewdeCasteljau( t, err );
    out << p.x << " " << p.y << std::endl;
  }

}

bool ParSpaceTrimmer::isOverQuad( DCTPFace *f, vec2d &p )
{
#ifdef OSG_UNION_TRI_QUAD
/*        std::cerr << "isoverface: " << f->orig_face[0]->coords << " "
                               << f->orig_face[1]->coords << " "
                               << f->orig_face[2]->coords << " "
                               << f->orig_face[3]->coords << std::endl;
        std::cerr << " point: " << p << std::endl;*/
        if ( p.x >= f->orig_face[0]->coords.x &&
             p.x <= f->orig_face[1]->coords.x &&
             p.y <= f->orig_face[0]->coords.y &&
             p.y >= f->orig_face[3]->coords.y )
#else
/*        std::cerr << "isoverface: " << f->orig_quad[0]->coords << " "
                               << f->orig_quad[1]->coords << " "
                               << f->orig_quad[2]->coords << " "
                               << f->orig_quad[3]->coords << std::endl;
        std::cerr << " point: " << p << std::endl;*/
        if ( p.x >= f->orig_quad[0]->coords.x &&
             p.x <= f->orig_quad[1]->coords.x &&
             p.y <= f->orig_quad[0]->coords.y &&
             p.y >= f->orig_quad[3]->coords.y )
#endif
                return true;
        else return false;
}

bool ParSpaceTrimmer::isNearQuad( DCTPFace *f, vec2d &p ) {

/*        std::cerr << "isoverface: " << f->orig_quad[0]->coords << " "
                               << f->orig_quad[1]->coords << " "
                               << f->orig_quad[2]->coords << " "
                               << f->orig_quad[3]->coords << std::endl;
        std::cerr << " point: " << p << std::endl;*/

#ifdef OSG_UNION_TRI_QUAD
        if ( ( p.x - f->orig_face[0]->coords.x > -DCTP_EPS * 2.0 ) &&
             ( p.x - f->orig_face[1]->coords.x < DCTP_EPS * 2.0 ) &&
             ( p.y - f->orig_face[0]->coords.y < DCTP_EPS * 2.0 ) &&
             ( p.y - f->orig_face[3]->coords.y > -DCTP_EPS * 2.0 ) )
#else
        if ( ( p.x - f->orig_quad[0]->coords.x > -DCTP_EPS * 2.0 ) &&
             ( p.x - f->orig_quad[1]->coords.x < DCTP_EPS * 2.0 ) &&
             ( p.y - f->orig_quad[0]->coords.y < DCTP_EPS * 2.0 ) &&
             ( p.y - f->orig_quad[3]->coords.y > -DCTP_EPS * 2.0 ) )
#endif
                return true;
        else return false;
}

bool ParSpaceTrimmer::isOverFace( DCTPFace *f, vec2d &p ) {
        return isOverQuad( f, p );
}

void ParSpaceTrimmer::getStartingFace( vec2d p )
{
	dctpfacevector::iterator fend = mesh->faces.end();
        dctpfacevector::iterator i;
        
	if( p.x < m_clMin.x ) p.x = m_clMin.x;
	else if( p.x > m_clMax.x ) p.x = m_clMax.x;
	if( p.y < m_clMin.y ) p.y = m_clMin.y;
	else if( p.y > m_clMax.y ) p.y = m_clMax.y;

	for( i = mesh->faces.begin(); i != fend; ++i )
	{
		if( isOverFace( *i, p ) )
		{
			state.state = TrimState::OVER_FACE;
			state.f = *i;
			return;
		}
	}//end for( i

	for( i = mesh->faces.begin(); i != fend; ++i )
	{
		if( isNearQuad( *i, p ) )
		{
			state.state = TrimState::OVER_FACE;
			state.f = *i;
			return;
		}
	}//end for( i

	//doncha wanna get here
	std::cerr << "Couldn't find starting face!" << std::endl;

	throw ParSpaceTrimmerError( ERR_NO_STARTING_FACE );
}

bool ParSpaceTrimmer::isOnSection( vec2d &p1, vec2d &p2, vec2d &p ) {

  vec2d norm, t;
  double dist;
  double par;
  double lab = sqrt( ( p1.x - p2.x ) * ( p1.x - p2.x ) +
                     ( p1.y - p2.y ) * ( p1.y - p2.y ) );
  norm.x = -( p2.y - p1.y ) / lab; // This is a normal -> rotated by 90 degrees
  norm.y = ( p2.x - p1.x ) / lab;

  t.x = p.x - p1.x;
  t.y = p.y - p1.y;
  dist = t.x * norm.x + t.y * norm.y;
  if ( fabs( dist ) < DCTP_EPS ) {
    // it is on the line at least, check if it's on the section
    if ( fabs( p2.x - p1.x ) > DCTP_EPS )
      par = ( p.x - p1.x )/ ( p2.x - p1.x );
    else
      par = ( p.y - p1.y )/ ( p2.y - p1.y );
    if ( par < -DCTP_EPS || par > ( 1 + DCTP_EPS) )
      return false;
    else
      return true;
  }
  else
    return false;
}

bool ParSpaceTrimmer::isOnEdge( DCTPEdge *e, vec2d &p, DCTPVertex* &v ) {
        DCTPVertex *v1, *v2;
        e->getVertices( v1, v2 );
        vec2d v1coords( v1->coords.x, v1->coords.y );
        vec2d v2coords( v2->coords.x, v2->coords.y );
        if ( p == v1coords ) {
                v = v1;
                return true;
        }
        if ( p == v2coords ) {
                v = v2;
                return true;
        }
        v = NULL;
        return isOnSection( v1coords, v2coords, p );
}

void *ParSpaceTrimmer::isOnFaceBoundary( DCTPFace *f, vec2d &p, bool& isedge )
//if returned ptr is not NULL,
//isedge = false means (void*) points to a vertex
//isedge = true means (void*) points to an edge
{
	std::vector< DCTPEdge *>::iterator eend = f->edges.end();

	for( std::vector< DCTPEdge *>::iterator i = f->edges.begin(); i != eend; ++i )
	{
		DCTPVertex *v;
		
		isedge = isOnEdge( *i, p, v );
		if ( isedge )
		{
			if ( v )
			{
				isedge = false;
				return v;
			}
			else
			{
				return *i;
			}
		}
	}//end for i

	return NULL;
}

void ParSpaceTrimmer::initializeStartState( bezier2ddeque& tc ) {
//	std::cerr << "initializeStartState: tc.size(): " << tc.size() << std::endl;

	BezierCurve2D &b = tc[ 0 ];
	vec2dvector &cp = b.getControlPointVector(); //std::cerr << "got control point vector " << std::endl;
	vec2d &p = cp[ 0 ]; // p = first point of the curveseq.
	getStartingFace( p ); //std::cerr << "got starting face " << std::endl;

	start_face = NULL;
	bool isedge;
	void *ptr;
	if( ( ptr = isOnFaceBoundary( state.f, p, isedge ) ) )
	{
		state.state = TrimState::IN_VERTEX;
		if ( !isedge ) //startin' from a vertex
		{
//			std::cerr << "start is in edge" << std::endl;
			state.v = (DCTPVertex*)ptr;
		}
		else
		{
//			std::cerr << "start is on boundary" << std::endl;
			//starting on an edge: split
			vec3d p3d = p;
//			state.v = mesh->SplitEdge( (DCTPEdge*)ptr, p3d );
			state.v = mesh->findVertex( p3d );
			if( state.v == NULL )
			{
				state.v = new DCTPVertex;
				state.v->coords = p3d;
				state.v->faces = ( ( DCTPEdge* ) ptr )->faces;
//				std::cerr << "2-" << ptr << " " << state.v << std::endl;
				state.v->edges.push_back( ( DCTPEdge* ) ptr );
			}
			state.v->vertexinfo = (void*) 1;
		}
	}
	else
	{
//		std::cerr << "start is in face" << std::endl;
		//TrimSegment tsg;
		//tsg.start = NULL;
		//state.f->trimseg.push_back( tsg );
		start_face = state.f;
	}

}

void ParSpaceTrimmer::initializeStartState2( unsigned int uiLoop, std::vector< DCTPVertex* > &el ) {
//	std::cerr << "initializeStartState: tc.size(): " << tc.size() << std::endl;

	const unsigned int	cui_size = ( *pvccrd )[ uiLoop ].size( );
	vec2d				&p = ( *pvccrd )[ uiLoop ][ cui_size - 1 ];
	vec3d				&rcl_s = ( *m_pvvclSewed )[ uiLoop ][ cui_size - 1 ];
#ifdef OSG_FORCE_NO_T_VERTICES
/* #ifndef OSG_CREATE_NORMAL_MAPS
	vec3d				&rcl_n = ( *m_pvvclNormals )[ uiLoop ][ cui_size - 1 ];
 #endif*/
#endif

/*	if( p.x < m_clMin.x ) p.x = m_clMin.x;
	else if( p.x > m_clMax.x ) p.x = m_clMax.x;
	if( p.y < m_clMin.y ) p.y = m_clMin.y;
	else if( p.y > m_clMax.y ) p.y = m_clMax.y;*/

	getStartingFace( p ); //std::cerr << "got starting face " << std::endl;

//	std::cerr << "init: " << p << " " << rcl_s << std::endl;

	start_face = NULL;
	bool isedge;
	void *ptr;
	if( ( ptr = isOnFaceBoundary( state.f, p, isedge ) ) )
	{
		state.state = TrimState::IN_VERTEX;
		if ( !isedge ) //startin' from a vertex
		{
//			std::cerr << "start is in vertex" << std::endl;
			state.v = ( DCTPVertex* )ptr;
		}
		else
		{
//			std::cerr << "start is on edge" << std::endl;
			//starting on an edge: split
			vec3d p3d = p;
			state.v = mesh->SplitEdge( ( DCTPEdge* )ptr, p3d );
//			std::cerr << state.v->coords << std::endl;
		}
#ifdef OSG_FORCE_NO_T_VERTICES
		SPosNorm	*pt_s = new SPosNorm;
		pt_s->clPos = rcl_s;
/* #ifndef OSG_CREATE_NORMAL_MAPS
		pt_s->clNorm = rcl_n;
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
		pt_s->uiNum = m_uiPosCnt;
 #endif
		state.v->vertexinfo = ( void* ) pt_s;
#else
		vec3d *pcl_s = new vec3d( rcl_s );
		state.v->vertexinfo = ( void* ) pcl_s;
#endif
//		std::cerr << state.v->vertexinfo << std::endl;
		el.push_back( state.v );
	}
	else
	{
//		std::cerr << "start is in face" << std::endl;
		start_face = state.f;
		DCTPVertex	*pcl_v = mesh->AddVertex( vec3d( p ) );
#ifdef OSG_FORCE_NO_T_VERTICES
		SPosNorm	*pt_s = new SPosNorm;
		pt_s->clPos = rcl_s;
/* #ifndef OSG_CREATE_NORMAL_MAPS
		pt_s->clNorm = rcl_n;
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
		pt_s->uiNum = m_uiPosCnt;
 #endif
		pcl_v->vertexinfo = ( void* ) pt_s;
#else
		vec3d		*pcl_s = new vec3d( rcl_s );
		pcl_v->vertexinfo = pcl_s;
#endif
//		std::cerr << pcl_v->vertexinfo << std::endl;
		el.push_back( pcl_v );
	}

}

bool ParSpaceTrimmer::setMinIntersectionWithFace( BezierCurve2D &b ) {
        ip = 1.5;
        bool intersection = false;
        std::vector< DCTPEdge *>::iterator eend = state.f->edges.end();
        /*
         * FIXME: bug #n: check for degenerate bezier
         * and throw an exception :-\
         */
         vec2dvector &cptemp = b.getControlPointVector();
         if ( cptemp.size() == 2 && cptemp[ 0 ] == cptemp[ 1 ] )
           throw ParSpaceTrimmerError( ERR_DEGENERATE_BEZIER );


///        dumpcontrolpoints( b );
///        std::cerr << "Face id: " << state.f->id << std::endl;
        for( std::vector< DCTPEdge *>::iterator i = state.f->edges.begin();
             i != eend; ++i ) {
             dvector res;
             DCTPVertex *v1, *v2;
             (*i)->getVertices( v1, v2 );
             vec2d v1coords( v1->coords.x, v1->coords.y );
             vec2d v2coords( v2->coords.x, v2->coords.y );

             int err;
			 err = b.intersection( res, v1coords, v2coords );
             if ( err < 0 ) {
                std::cerr << "Brutal error " << err << " in: " <<
                        "ParSpaceTrimmer::setMinIntersectionWithFace" << std::endl;
                throw ParSpaceTrimmerError( ERR_SET_MININTERSECTION );
             }
///             std::cerr << " v1: " << v1coords << " v2: " << v2coords << std::endl;
///             if ( res.size() ) std::cerr << "minintersectiongeci res.size(): " << res.size() << " res[0]: " << res[0] << std::endl;
///             else std::cerr <<"minintersectiongeci lofasz se tortent" << std::endl;
             // we don't store intersections at t=0 because when going from IN_VERTEX->OVER_FACE
             // we pretend early to be OVER_FACE (when actually still in the vertex)
			 if( res.size( ) != 0 )
			 {
				const vec2d	ccl_temp = b.computewdeCasteljau( res[ 0 ], err );
//				std::cerr << ccl_temp << cptemp[ 0 ] << res[ 0 ]<< std::endl;
	            if( ccl_temp != cptemp[ 0 ] && res[ 0 ] < ip )
				{
///                 std::cerr << " doing da limbo dancez0rz " << std::endl;
					ip = res[ 0 ];
					ie = *i;
					intersection = true;
			    }
			    // FIXME: akos beleganyolt
				else if ( res.size( ) >= 2 && ccl_temp == cptemp[ 0 ] && res[ 1 ] < ip )
				{
					const vec2d	ccl_temp2 = b.computewdeCasteljau( res[ 0 ], err );
					if( ccl_temp2 != cptemp[ 0 ] )
					{
///						std::cerr << " doing da REAL FUNKY limbo dancez0rz " << res[ 1 ] << std::endl;
						ip = res[ 1 ];
						ie = *i;
						intersection = true;
					}
				}
			 }
             // FIXME: akos beleganyolasanak itt a vege
        }
        return intersection; //true means ip & ie are valid
}

bool ParSpaceTrimmer::setMinIntersectionWithFace2( vec2d clAct, vec2d clNext )
{
	ip = 1.5;
	bool intersection = false;
	std::vector< DCTPEdge *>::iterator eend = state.f->edges.end();
	/*
	 * FIXME: bug #n: check for degenerate bezier
	 * and throw an exception :-\
	 */
	if( clAct == clNext )
		throw ParSpaceTrimmerError( ERR_DEGENERATE_BEZIER );

	for( std::vector< DCTPEdge *>::iterator i = state.f->edges.begin();
		 i != eend; ++i )
	{
		double res;
		DCTPVertex *v1, *v2;
		(*i)->getVertices( v1, v2 );
		vec2d v1coords( v1->coords.x, v1->coords.y );
		vec2d v2coords( v2->coords.x, v2->coords.y );

		if( doIntersect( clAct, clNext, v1coords, v2coords, res ) )
		{
			if( ( res - 1.0 < DCTP_EPS ) && ( res > 1e-16 ) )
			{
//				std::cerr << "intersection: " << res << std::endl;
				if( res < ip )
				{
					ie = *i;
					ip = res;
				}
				intersection = true;
			}
		}
	}
	return intersection; //true means ip & ie are valid
}

bool ParSpaceTrimmer::StoreCurveApproximation(
                BezierCurve2D &bc,
                double t,
                std::vector< DCTPVertex* > &el )
{
	bezier2dvector	nc; //new curves
	bool					rest_left;
	double					&norm = state.f->norm;
	vec2dvector				approx;

//	std::cerr << "StoreCurveApproximation" << std::endl;

	if ( 1.0 - t < DCTP_EPS )
	{
		rest_left = false;
		vec2dvector vcp = bc.getControlPointVector( );
//		std::cerr <<vcp.size( ) << " ";
//		std::cerr << vcp[ 0 ] << " " << vcp[ 1 ] << " ";
		nc.push_back( bc );
		vcp = nc[ 0 ].getControlPointVector( );
//		std::cerr << vcp.size( ) << std::endl;
	}
	else
	{
		bc.subDivision( t, nc );
//std::cerr << "lofasz: " << nc.size() << std::endl;
		bc = nc[ 1 ];//nc[0] holdz da first part
		rest_left = true;
	}

//	std::cerr << "norm = " << norm << std::endl;

	if( m_bMaxLength )
	{
		nc[ 0 ].approximateLength( approx, norm );
	}
	else
	{
		nc[ 0 ].approximate( approx, norm );
	}
	if( approx.size( ) >= 2 )
	{
		vec2dvector::iterator ae = approx.end();

		for( vec2dvector::iterator i = approx.begin() + 1; i != ae; ++i )
		{
			i->x = ROUND_EPS * floor( i->x / ROUND_EPS + 0.5 );
			i->y = ROUND_EPS * floor( i->y / ROUND_EPS + 0.5 );
//			std::cerr << "inserting vertex in edge loop " << *i << std::endl;
			DCTPVertex	*pcl_new = mesh->findVertex( *i );
			if( pcl_new == NULL )
			{
				pcl_new = new DCTPVertex;
				pcl_new->coords = *i;
			}
//			std::cerr << "el.push_back " << ( void* ) pcl_new << std::endl;
			el.push_back( pcl_new );
		}
	}

	return rest_left;
}

/*bool ParSpaceTrimmer::StoreCurveToFace(
                BezierCurve2D &bc,
                double t,
                DCTPVertex *v ) {
//v is not NULL if curve ends in a node
//in state.f is the face to store into
///        std::cerr.precision( DCTP_PRECISION );
///        std::cerr << " storecurve: t:" << t << " v: " << (void*)v << std::endl;
	bezier2dvector	nc; //new curves
	bool			rest_left;

	if ( 1.0 - t < DCTP_EPS )
	{
		rest_left = false;
		nc.push_back( bc );
	}
	else
 {
		bc.subDivision( t, nc );
///std::cerr << "lofasz: " << nc.size() << std::endl;
		bc = nc[ 1 ];//nc[0] holdz da first part
		rest_left = true;
	}


        TrimSegment& ts =
                state.f->trimseg[ state.f->trimseg.size() - 1 ];//should exist!
        ts.trimbeziers.push_back( nc[ 0 ] );
        if ( v ) ts.end = v;
	return rest_left;
}*/

bool ParSpaceTrimmer::goingOutOnEdge( BezierCurve2D &bc,
                                      DCTPVertex* &v, bool &feu,
                					  std::vector< DCTPVertex* > &el ) {
        bool on_edge = false;
///        std::cerr <<"goingout in..." << std::endl;
        std::vector< DCTPEdge *>::iterator eend = state.v->edges.end();
        for( std::vector< DCTPEdge *>::iterator i = state.v->edges.begin();
             i != eend; ++i )
		{
                DCTPVertex *v1,*v2;
                (*i)->getVertices( v1, v2 );
                dvector res;
//                std::cerr << "goin'outonedge calling intersection and houston ;)" << std::endl;
//                dumpcontrolpoints( bc );
//                std::cerr << "v1: " << v1->coords << " v2: " << v2->coords << std::endl;
                vec2d v1coords( v1->coords.x, v1->coords.y );
                vec2d v2coords( v2->coords.x, v2->coords.y );

                vec2dvector &cptemp = bc.getControlPointVector();
                if ( cptemp.size() == 2 && cptemp[ 0 ] == cptemp[ 1 ] )
                    throw ParSpaceTrimmerError( ERR_DEGENERATE_BEZIER );

                int r = bc.intersection( res, v1coords, v2coords );
//                std::cerr << "intersection result: " << r << "resultvector size: " << res.size() << std::endl;
//                if ( res.size() ) std::cerr <<"first intersec: " << res[0] << std::endl;
				if( res.size( ) == 2 )
				{
//					std::cerr << res[ 1 ] << std::endl;
					if( ( r == 2 ) && ( res[ 1 ] - 1.0 > -DCTP_EPS ) ) r = 1;
				}
                switch ( r ) {
                        case 1:
							{
								vec2dvector &cp = bc.getControlPointVector();
								//std::cerr << "cp.size(): " << cp.size() << std::endl;
								//std::cerr << "cp[0]: " << cp[ 0 ] << "cp[1]: " << cp[ 1] << std::endl;
								vec3d cp3d( ROUND_EPS * floor( cp[ cp.size() - 1 ].x / ROUND_EPS + 0.5 ),
											ROUND_EPS * floor( cp[ cp.size() - 1 ].y / ROUND_EPS + 0.5 ) );
//								vec3d cp3d( cp[ cp.size( ) - 1 ].x, cp[ cp.size( ) - 1 ].y );
//                              v = mesh->SplitEdge( *i, cp3d );
								v = mesh->findVertex( cp3d );
								if( v == NULL )
								{
									v = new DCTPVertex;
									v->coords = cp3d;
									v->faces = ( *i )->faces;
									v->edges.push_back( *i );
//									std::cerr << "3-" << *i << " " << v << std::endl;
								}
								if( m_bMaxLength )
								{
									state.f = state.v->faces[ 0 ];
									StoreCurveApproximation( bc, 1.0, el );
								}
								else
								{
//									std::cerr << "el.push_back 2 " << ( void* ) v << std::endl;
									el.push_back( v );
								}
                                feu = true;
								return true;
							}
                        case 2:
							{
								bezier2dvector nc; //new curves

//	                            std::cerr << "res.size(): " << res.size() << std::endl;
//								dumpcontrolpoints( bc );
//								std::cerr << " res[0]: " << res[ 0 ] << " res[1]: " << res[ 1 ] << std::endl;
//								std::cerr << " lineseg: " << v1coords << " " << v2coords << std::endl;

								bc.subDivision( res[ 1 ], nc );
//								std::cerr << "LOFASZBAZMEG " << nc.size() << std::endl;
								bc = nc[ 1 ];//nc[0] holdz da 2nd part
//								std::cerr << "GECILADA" << std::endl;
								vec2dvector &cp = bc.getControlPointVector();
//		                        if ( cp[ 0 ] == v1coords ) v = v1;
//		                        else v = v2;
//	                            std::cerr << "cp[0]: " << cp[0] << " v1: " << v1c << " v2: " << v2c << std::endl;
		                        //FIXME:direct orig edge into v
								vec2dvector &cp2 =
										nc[ 0 ].getControlPointVector();

								if ( cp2[ 0 ] == v1coords ) v = v2;
								else v = v1;
								if( m_bMaxLength )
								{
									state.f = state.v->faces[ 0 ];
									StoreCurveApproximation( nc[ 0 ], 1.0, el );
								}
								else
								{
//									std::cerr << "el.push_back 3 " << ( void* ) v << std::endl;
									el.push_back( v );
//		                            vec3d cp203d( cp2[ 0 ].x, cp2[ 0 ].y );
/*				                    if ( mesh->directEdge( cp203d, v->coords ) ) {
										std::cerr << "directEdge ERROR!" << std::endl;
											throw ParSpaceTrimmerError( ParSpaceTrimmerError::ERR_DIRECTEDGE );
									}*/
								}
                                feu = false;
                                return true;
							}
                }
		}
        return false;
}

bool ParSpaceTrimmer::goingOutOnEdge2( vec2d clAct, vec2d clNext,
                                       DCTPVertex* &v, bool &feu,
                					   std::vector< DCTPVertex* > &el )
{
	bool on_edge = false;
//	std::cerr <<"goingout in..." << std::endl;
	std::vector< DCTPEdge *>::iterator eend = state.v->edges.end();
	for( std::vector< DCTPEdge *>::iterator i = state.v->edges.begin();
		 i != eend; ++i )
	{
		if( ( *i )->faces.size( ) != 0 )
		{
			DCTPVertex *v1,*v2;
			(*i)->getVertices( v1, v2 );
			double res;
//			std::cerr << "goin'outonedge calling intersection and houston ;)" << std::endl;
//			dumpcontrolpoints( bc );
//			std::cerr << "v1: " << v1->coords << " v2: " << v2->coords << std::endl;
			vec2d v1coords( v1->coords.x, v1->coords.y );
			vec2d v2coords( v2->coords.x, v2->coords.y );

			if( clAct == clNext )
				throw ParSpaceTrimmerError( ERR_DEGENERATE_BEZIER );

			if( doIntersect( clAct, clNext, v1coords, v2coords, res ) )
			{
				if( res - 1.0 > DCTP_EPS )
				{
					vec3d cp3d( clNext.x, clNext.y );
					v = mesh->SplitEdge( *i, cp3d );
					if( v == NULL )
					{
						std::cerr << "SplitEdge Failed!" << std::endl;
						std::cerr << v1coords << cp3d << v2coords << std::endl;
					}
					else
					{
//						el.push_back( v );
						feu = true;
						return true;
					}
				}
				else if( res - 1.0 < -DCTP_EPS )
				{
					vec3d cp3p = clAct + ( clNext - clAct ) * res;
					if( ( cp3p - v1coords ).quad_size( ) <
						( cp3p - v2coords ).quad_size( ) )
						v = v1;
					else
						v = v2;
//					el.push_back( v );
					feu = ( cp3p == clNext );
					ip = res;
					return true;
				}
				else
				{
					vec3d cp3d( clNext.x, clNext.y );
					if( ( cp3d - v1coords ).quad_size( ) <
						( cp3d - v2coords ).quad_size( ) )
						v = v1;
					else
						v = v2;
//					el.push_back( v );
					feu = true;
					return true;
				}
			}
		}
	}
//	std::cerr << "going out..." << std::endl;
	return false;
}

DCTPFace* ParSpaceTrimmer::getContinuingFace( BezierCurve2D &bc ) {
//state.v shulda hold the vertex to investigate
        int err;
        double high = 1.0, low = 0.0;
//        std::cerr <<" computing stuph.." << std::endl;
        const vec2d cp0 = bc.getControlPointVector()[ 0 ];
        vec2d p = bc.computewdeCasteljau( high, err );
		if( p.x < m_clMin.x ) p.x = m_clMin.x;
		else if( p.x > m_clMax.x ) p.x = m_clMax.x;
		if( p.y < m_clMin.y ) p.y = m_clMin.y;
		else if( p.y > m_clMax.y ) p.y = m_clMax.y;
        while( high - low > DCTP_EPS )
        {
//			std::cerr << ".";
//          const double mid = ( high + low ) * 0.5;
          double mid = 1 * DCTP_EPS / sqrt( ( p - cp0 ).quad_size( ) );
		  mid = osgMin( 0.9, osgMax( 0.1, mid ) );
		  mid = low + ( high - low ) * mid;
          vec2d ptmp = bc.computewdeCasteljau( mid, err );
		  if( ptmp.x < m_clMin.x ) ptmp.x = m_clMin.x;
		  else if( ptmp.x > m_clMax.x ) ptmp.x = m_clMax.x;
		  if( ptmp.y < m_clMin.y ) ptmp.y = m_clMin.y;
		  else if( ptmp.y > m_clMax.y ) ptmp.y = m_clMax.y;
          if( cp0 == ptmp )
          {
			if( p == ptmp )
			{
			  break;
			}
            low = mid;
          }
          else
          {
            high = mid;
            p = ptmp;
          }
        }
//		std::cerr << std::endl;
//		std::cerr << cp0 << state.v->coords << p << std::endl;
//        std::cerr << " computed stuph: " << k << std::endl;
///        dumpcontrolpoints( bc );
        std::vector< DCTPFace * >::iterator fe = state.v->faces.end(); //faces end
        std::vector< DCTPFace * >::iterator i;
//        std::cerr <<" state.v->faces.size(): " << state.v->faces.size() << std::endl;
		for( i = state.v->faces.begin(); i != fe; ++i )
		{
             if ( isOverFace( *i, p ) ) return *i;
        }
		for( i = state.v->faces.begin(); i != fe; ++i )
		{
             if ( isNearQuad( *i, p ) ) return *i;
        }
		// nothing found, so we do a global serach
        getStartingFace( p );
		return state.f;
}

DCTPFace* ParSpaceTrimmer::getContinuingFace2( vec2d clAct, vec2d clNext )
{
//state.v shulda hold the vertex to investigate
//	int err;
	double high = 1.0, low = 0.0;
//	std::cerr <<" computing stuph.." << std::endl;

	if( clAct.x < m_clMin.x ) clAct.x = m_clMin.x;
	else if( clAct.x > m_clMax.x ) clAct.x = m_clMax.x;
	if( clAct.y < m_clMin.y ) clAct.y = m_clMin.y;
	else if( clAct.y > m_clMax.y ) clAct.y = m_clMax.y;
	if( clNext.x < m_clMin.x ) clNext.x = m_clMin.x;
	else if( clNext.x > m_clMax.x ) clNext.x = m_clMax.x;
	if( clNext.y < m_clMin.y ) clNext.y = m_clMin.y;
	else if( clNext.y > m_clMax.y ) clNext.y = m_clMax.y;

/*	vec2d p = clAct + ( clNext - clAct ) * high;

	while( high - low > DCTP_EPS )
	{
		const double mid = ( high + low ) * 0.5;
		const vec2d ptmp = clAct + ( clNext - clAct ) * mid;
		if( clAct == ptmp )
		{
			low = mid;
		}
		else
		{
			high = mid;
			p = ptmp;
		}
	}*/

	vec2d p;

	p = clNext - clAct;
	if( fabs( p.x ) > DCTP_EPS )
	{
		p.x = DCTP_EPS * p.x / fabs( p.x );
	}
	if( fabs( p.y ) > DCTP_EPS )
	{
		p.y = DCTP_EPS * p.y / fabs( p.y );
	}
	p += clAct;

//        std::cerr << " computed stuph: " << k << std::endl;
///        dumpcontrolpoints( bc );
        std::vector< DCTPFace * >::iterator fe = state.v->faces.end(); //faces end
//		std::cerr << clAct << p << clNext << std::endl;
//        std::cerr <<" state.v: " << state.v->coords << " " << state.v->faces.size() << std::endl;
        for( std::vector< DCTPFace * >::iterator i = state.v->faces.begin();
             i != fe; ++i ) {
/*			std::cerr << "isoverface: " << (*i)->orig_quad[0]->coords << " "
								   << (*i)->orig_quad[1]->coords << " "
								   << (*i)->orig_quad[2]->coords << " "
								   << (*i)->orig_quad[3]->coords << std::endl;*/
             if ( isOverFace( *i, p ) ) return *i;
        }
		// nothing found, so we do a global serach
        getStartingFace( p );
		return state.f;
}

bool ParSpaceTrimmer::StateTransition( BezierCurve2D &b, std::vector< DCTPVertex* > &el ) {
        if ( state.state == TrimState::OVER_FACE ) {
#ifdef OSG_ADAPTIVE_QUAD_TREE
			if( state.f->norm < DCTP_EPS )
			{
				while( m_pclQuadTree->computeApproximationError( state.f ) > m_pclQuadTree->error_epsilon )
				{
//					std::cerr << "subdivide ";
//					state.f->dump_triangle( );
//					std::cerr << "subdividing face " << ( void* ) state.f << " because of error " << m_pclQuadTree->computeApproximationError( state.f ) << std::endl;
					mesh->SubdivideQuad( state.f );
					m_pclQuadTree->finishSubdivisions( state.f );
					if( !isOverFace( state.f, b.getControlPointVector( )[ 0 ] ) )
					{
						if( isOverFace( mesh->faces[ mesh->faces.size( ) - 3 ], b.getControlPointVector( )[ 0 ] ) )
						{
							state.f = mesh->faces[ mesh->faces.size( ) - 3 ];
						}
						else if( isOverFace( mesh->faces[ mesh->faces.size( ) - 2 ], b.getControlPointVector( )[ 0 ] ) )
						{
							state.f = mesh->faces[ mesh->faces.size( ) - 2 ];
						}
						else
						{
							state.f = mesh->faces[ mesh->faces.size( ) - 1 ];
						}
					}
				}
	            state.f->norm = m_pclQuadTree->error_epsilon / m_pclQuadTree->computeBilinearNorm( state.f );
//				std::cerr << state.f->norm << std::endl;
			}
#endif
//        std::cerr << " now over face..." << std::endl;
                //note: setMinIntersection... sets ie & ip member varz
                if ( setMinIntersectionWithFace( b ) ) {//intersection exists
                        int err;
                        vec3d p = b.computewdeCasteljau( ip, err );
//                        std::cerr << " ip: " << ip << " p: " << p << std::endl;
//                        DCTPVertex *vp = mesh->SplitEdge( ie, p );
						DCTPVertex *vp = mesh->findVertex( p );
						if( vp == NULL )
						{
							vp = new DCTPVertex;
							vp->coords = p;
							vp->faces = ie->faces;
							vp->edges.push_back( ie );
/*							std::cerr << "5-" << ie << " " << vp << std::endl;
							if( ( ( unsigned int ) vp ) == 0x2ec94840 )
							{
								std::cerr << p << std::endl;
							}*/
						}
                        if ( !vp ) {
                                std::cerr << "Brutal error I. in: "
                                     << "ParSpaceTrimmer::StateTransition"
                                     << std::endl;
                                throw ParSpaceTrimmerError( ERR_STATETRANSITION_I );
                        }
						vp->vertexinfo = (void*) 1;
                        state.state = TrimState::IN_VERTEX;
                        state.v = vp;
						return StoreCurveApproximation( b, ip, el );
//						el.push_back( vp );
                }
                //no intersection
///                std::cerr << " no intersection, baby !!! " << std::endl;
				return StoreCurveApproximation( b, 1.0, el );
        }
        else {//TrimState::IN_VERTEX state
#ifdef OSG_ADAPTIVE_QUAD_TREE
			bool		b_sub = true;
			vec2d		cl_v2d;
			bool		b_dummy;
			DCTPVertex	*pcl_find;

			cl_v2d.x = state.v->coords.x;
			cl_v2d.y = state.v->coords.y;

			while( b_sub )
			{
				b_sub = false;
				for( unsigned int ui_face = 0; ui_face < state.v->faces.size( ); ++ui_face )
				{
					DCTPFace	*pcl_face = state.v->faces[ ui_face ];
					if( pcl_face->norm < DCTP_EPS )
					{
						if( m_pclQuadTree->computeApproximationError( pcl_face ) > m_pclQuadTree->error_epsilon )
						{
//							std::cerr << "subdivide ";
//							pcl_face->dump_triangle( );
//							std::cerr << "subdividing face " << ( void* ) pcl_face << " because of error " << m_pclQuadTree->computeApproximationError( pcl_face ) << std::endl;
							mesh->SubdivideQuad( pcl_face );
							m_pclQuadTree->finishSubdivisions( pcl_face );
							b_sub = true;
							if( state.v->edges.size( ) <= 1 )
							{
								pcl_find = mesh->findVertex( state.v->coords );
								if( pcl_find )
								{
//									std::cerr << "delete vertex1:" << ( void* ) state.v << std::endl;
//									delete state.v;
//									std::cerr << pcl_find->edges.size( ) << std::endl;
									state.v = pcl_find;
								}
								else
								{
									// update state.v->faces and state.v->edges
									if( !isNearQuad( pcl_face, cl_v2d ) )
									{
										state.v->faces[ ui_face ] = state.v->faces[ state.v->faces.size( ) - 1 ];
										state.v->faces.pop_back( );
										--ui_face;
									}
									else 
									{
										state.v->edges[ 0 ] = ( DCTPEdge* )
											isOnFaceBoundary( state.v->faces[ ui_face ], cl_v2d, b_dummy );
									}
									if( isNearQuad( mesh->faces[ mesh->faces.size( ) - 3 ], cl_v2d ) )
									{
										state.v->faces.push_back( mesh->faces[ mesh->faces.size( ) - 3 ] );
										state.v->edges[ 0 ] = ( DCTPEdge* )
											isOnFaceBoundary( mesh->faces[ mesh->faces.size( ) - 3 ], cl_v2d, b_dummy );
									}
									if( isNearQuad( mesh->faces[ mesh->faces.size( ) - 2 ], cl_v2d ) )
									{
										state.v->faces.push_back( mesh->faces[ mesh->faces.size( ) - 2 ] );
										state.v->edges[ 0 ] = ( DCTPEdge* )
											isOnFaceBoundary( mesh->faces[ mesh->faces.size( ) - 2 ], cl_v2d, b_dummy );
									}
									if( isNearQuad( mesh->faces[ mesh->faces.size( ) - 1 ], cl_v2d ) )
									{
										state.v->faces.push_back( mesh->faces[ mesh->faces.size( ) - 1 ] );
										state.v->edges[ 0 ] = ( DCTPEdge* )
											isOnFaceBoundary( mesh->faces[ mesh->faces.size( ) - 1 ], cl_v2d, b_dummy );
									}
								}
							}
						}
						else
						{
				            pcl_face->norm = m_pclQuadTree->error_epsilon / m_pclQuadTree->computeBilinearNorm( pcl_face );
//							std::cerr << ( void* ) pcl_face << ":" << pcl_face->norm << std::endl;
						}
					}
				}
			}
#endif
//        std::cerr << " now in vertex..." << std::endl;
        //snap startin' control point of outgoin' curve into startin' vertex
                vec2dvector &cp = b.getControlPointVector();
                cp[ 0 ].x = state.v->coords.x;
                cp[ 0 ].y = state.v->coords.y;
                bool feu; //full edge used
                DCTPVertex *v;
				unsigned int check;
				for( check = 1; check < cp.size( ); ++check )
				{
					if( cp[ check ] != cp[ 0 ] )
					{
						break;
					}
				}
				if( check == cp.size( ) )
				{
					return false;
				}
//                if ( cp.size() == 2 && cp[ 0 ] == cp[ 1 ] )
//                    return false;
                if ( goingOutOnEdge( b, v, feu, el ) ) {//goin' out on edge
                        //IN_VERTEX state remains, but...
                        state.v = v;
						//insert edge in edgeloop
//						std::cerr << "inserting vertex in edge loop " << state.v->coords << std::endl;
//						std::cerr << "el.push_back 4 " << ( void* ) v << std::endl;
//						el.push_back( v );
//						v->vertexinfo = ( void* ) 1;
                        if ( feu ) return false; //no part of curve remains
                        return true; //the opposite
                }
                DCTPFace *f = getContinuingFace( b );//get face curve is above
                if ( !f ) {
                        std::cerr << "Brutal error II. in: "
                             << "ParSpaceTrimmer::StateTransition" << std::endl;
                        throw ParSpaceTrimmerError( ERR_STATETRANSITION_II );
                }
//                TrimSegment tsg;
//                tsg.start = state.v;
//                f->trimseg.push_back( tsg );
                //OK - now we can pretend to be in OVER_FACE
                state.f = f;
                state.state = TrimState::OVER_FACE;
                return true; //all of the curve remains
        }
}

#ifdef OSG_FORCE_NO_T_VERTICES
// #ifdef OSG_CREATE_NORMAL_MAPS
bool ParSpaceTrimmer::StateTransition2( vec2d &rclAct, vec2d clNext, std::vector< DCTPVertex* > &el, vec3d &rclActS, vec3d clNextS )
/* #else
bool ParSpaceTrimmer::StateTransition2( vec2d &rclAct, vec2d clNext, std::vector< DCTPVertex* > &el, vec3d &rclActS, vec3d clNextS, vec3d &rclActN, vec3d clNextN )
 #endif*/
#else
bool ParSpaceTrimmer::StateTransition2( vec2d &rclAct, vec2d clNext, std::vector< DCTPVertex* > &el, vec3d &rclActS, vec3d clNextS )
#endif
{
	if( ( rclAct - clNext ).quad_size( ) < DCTP_EPS ) return false;

//	std::cerr << rclAct << " -> " << clNext << std::endl;
//	std::cerr << rclActS << " -> " << clNextS << std::endl;

	if( state.state == TrimState::OVER_FACE )
	{
//		std::cerr << " now over face..." << std::endl;
		//note: setMinIntersection... sets ie & ip member varz
		if( setMinIntersectionWithFace2( rclAct, clNext ) )
		{//intersection exists
//			int err;
			vec3d p = rclAct + ( clNext - rclAct ) * ip;
//			std::cerr << " ip: " << ip << " p: " << p << std::endl;
			DCTPVertex *vp = mesh->SplitEdge( ie, p );
//			std::cerr << vp->coords << std::endl;
			if( vp == NULL )
			{
				std::cerr << rclAct << " -> " << clNext << std::endl;
				DCTPVertex	*v1, *v2;
				ie->getVertices( v1, v2 );
				std::cerr << v1->coords << " -> " << v2->coords << std::endl;
				std::cerr << " ip: " << ip << " p: " << p << std::endl;
				std::cerr << "Brutal error I. in: "
					 << "ParSpaceTrimmer::StateTransition2"
					 << std::endl;
//				exit( -1 );
				throw ParSpaceTrimmerError( ERR_STATETRANSITION_I );
			}
			rclActS = rclActS + ( clNextS - rclActS ) * ip;
			if( vp->vertexinfo == NULL )
			{
#ifdef OSG_FORCE_NO_T_VERTICES
				SPosNorm	*pt_s = new SPosNorm;
				SPosNorm	*pt_ps = ( SPosNorm* ) el[ el.size( ) - 1 ]->vertexinfo;

				if( ( rclActS - clNextS ).quad_size( ) <= ( rclActS - pt_ps->clPos ).quad_size( ) )
				{
					pt_s->clPos = clNextS;
/* #ifndef OSG_CREATE_NORMAL_MAPS
					pt_s->clNorm = clNextN;
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
					pt_s->uiNum = m_uiPosCnt;
 #endif
				}
				else
				{
					pt_s->clPos = pt_ps->clPos;
/* #ifndef OSG_CREATE_NORMAL_MAPS
					pt_s->clNorm = pt_ps->clNorm;
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
					pt_s->uiNum = pt_ps->uiNum;
 #endif
				}
				vp->vertexinfo = ( void* ) pt_s;
#else
				vec3d *pcl_s = new vec3d( rclActS );
				vp->vertexinfo = ( void* ) pcl_s;
#endif
			}
			state.state = TrimState::IN_VERTEX;
			state.v = vp;
//			if( vec3d( rclAct ) != vp->coords )
			{
				mesh->AddEdge( el[ el.size( ) - 1 ], vp, 1 );
//				std::cerr << "add edge: " << el[ el.size( ) - 1 ]->coords << "->" << vp->coords << std::endl;
//				std::cerr << vp->vertexinfo << std::endl;
				el.push_back( vp );
			}
			rclAct.x = vp->coords.x;
			rclAct.y = vp->coords.y;
//			std::cerr << "rclAct:" << rclAct << std::endl;
//			std::cerr << "ip:" << ip << std::endl;
//			std::cerr << "rclAct == clNext " << ( rclAct == clNext ) << std::endl;
			return !( rclAct == clNext );
		}
		//no intersection
//		std::cerr << " no intersection, baby !!! " << std::endl;
		DCTPVertex *v = mesh->AddVertex( clNext );
		if( v->vertexinfo == NULL )
		{
#ifdef OSG_FORCE_NO_T_VERTICES
			SPosNorm	*pt_s = new SPosNorm;
			pt_s->clPos = clNextS;
/* #ifndef OSG_CREATE_NORMAL_MAPS
			pt_s->clNorm = clNextN;
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
			pt_s->uiNum = m_uiPosCnt;
 #endif
			v->vertexinfo = ( void* ) pt_s;
#else
			vec3d *pcl_s = new vec3d( clNextS );
			v->vertexinfo = ( void* ) pcl_s;
#endif
		}
		mesh->AddEdge( el[ el.size( ) - 1 ], v, 1 );
//		std::cerr << "add edge: " << el[ el.size( ) - 1 ]->coords << "->" << v->coords << std::endl;
//		std::cerr << v->vertexinfo << std::endl;
		el.push_back( v );
//		std::cerr << "clNext:" << clNext << std::endl;
		return false;
	}
	else
	{//TrimState::IN_VERTEX state
//		std::cerr << " now in vertex..." << std::endl;
		bool feu; //full edge used
		DCTPVertex *v;
		if( rclAct == clNext )
			return false;
		if( goingOutOnEdge2( rclAct, clNext, v, feu, el ) )
		{//goin' out on edge
			//IN_VERTEX state remains, but...
			if( feu )
			{
				rclActS = clNextS;
			}
			else
			{
				rclActS = rclActS + ( clNextS - rclActS ) * ip;
			}
			if( v->vertexinfo == NULL )
			{
#ifdef OSG_FORCE_NO_T_VERTICES
				SPosNorm	*pt_s = new SPosNorm;
				SPosNorm	*pt_ps = ( SPosNorm* ) el[ el.size( ) - 1 ]->vertexinfo;

				if( ( rclActS - clNextS ).quad_size( ) <= ( rclActS - pt_ps->clPos ).quad_size( ) )
				{
					pt_s->clPos = clNextS;
/* #ifndef OSG_CREATE_NORMAL_MAPS
					pt_s->clNorm = clNextN;
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
					pt_s->uiNum = m_uiPosCnt;
 #endif
				}
				else
				{
					pt_s->clPos = pt_ps->clPos;
/* #ifndef OSG_CREATE_NORMAL_MAPS
					pt_s->clNorm = pt_ps->clNorm;
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
					pt_s->uiNum = pt_ps->uiNum;
 #endif
				}
				v->vertexinfo = ( void* ) pt_s;
#else
				vec3d *pcl_s = new vec3d( rclActS );
				v->vertexinfo = ( void* ) pcl_s;
#endif
			}
			state.v = v;
			//insert edge in edgeloop
//			std::cerr << "inserting vertex in edge loop " << state.v->coords << std::endl;
//			if( vec3d( rclAct ) != v->coords )
			{
//				std::cerr << "direct edge: " << el[ el.size( ) - 1 ]->coords << "->" << v->coords << std::endl;
				mesh->directEdge( el[ el.size( ) - 1 ]->coords, v->coords );
//				std::cerr << v->vertexinfo << std::endl;
				el.push_back( v );
			}
			rclAct.x = v->coords.x;
			rclAct.y = v->coords.y;
//			std::cerr << "rclAct:" << rclAct << std::endl;
			return ( !feu );
		}
		DCTPFace *f = getContinuingFace2( rclAct, clNext );//get face curve is above
		if( f == NULL )
		{
			std::cerr << "Brutal error II. in: "
				 << "ParSpaceTrimmer::StateTransition2" << std::endl;
				 exit( -1 );
			throw ParSpaceTrimmerError( ERR_STATETRANSITION_II );
		}
		//OK - now we can pretend to be in OVER_FACE
		state.f = f;
		state.state = TrimState::OVER_FACE;
		return true; //all of the curve remains
	}
}

/*void ParSpaceTrimmer::mergeCurve( void ) {
///        std::cerr << "mergeCurve started, LOFASZPICSASEGG" << std::endl;
        if ( !start_face ) return;
///        std::cerr << "van startfacem, te geciladafasszopokocsog!!! " << std::endl;
        std::vector<TrimSegment>::iterator s = NULL, e = NULL;
        for( std::vector<TrimSegment>::iterator i = start_face->trimseg.begin();
             i != start_face->trimseg.end(); ++i ) {
             if ( !i->start ) { s = i; } //std::cerr << "ParSpaceTrimmer::mergeCurve start found: " << std::endl; }
             if ( !i->end ) { e = i; } //std::cerr << "ParSpaceTrimmer::mergeCurve end found: " << std::endl; }
        }
///        if ( e == s ) std::cerr <<" basszajba, GECZI !!! " << std::endl;
        if ( e == NULL ) throw ParSpaceTrimmerError( ParSpaceTrimmerError::ERR_NO_MERGECURVE_START );
        if ( s == NULL ) throw ParSpaceTrimmerError( ParSpaceTrimmerError::ERR_NO_MERGECURVE_END );
        e->end = s->end;
//        std::cerr << "e->trimbeziers.size: " << e->trimbeziers.size() << " s->trimbeziers.size(): " << s->trimbeziers.size() << std::endl;
        e->trimbeziers.insert( e->trimbeziers.end(), s->trimbeziers.begin(),
                               s->trimbeziers.end() );
        start_face->trimseg.erase( s );
}*/

void ParSpaceTrimmer::processCurve( bezier2ddeque &tc, std::vector< DCTPVertex* > &el ) {

        while ( !tc.empty() ) {
                BezierCurve2D bc = tc.front();
                tc.pop_front();
                if ( StateTransition( bc, el ) ) tc.push_front( bc );
        }
}

void ParSpaceTrimmer::processCurve2( unsigned int uiLoop, std::vector< DCTPVertex* > &el )
{
	const unsigned int	cui_size = ( *pvccrd )[ uiLoop ].size( );
	vec2d				cl_act = ( *pvccrd )[ uiLoop ][ cui_size - 1 ];
	vec3d				cl_act_s = ( *m_pvvclSewed )[ uiLoop ][ cui_size - 1 ];
#ifdef OSG_FORCE_NO_T_VERTICES
/* #ifndef OSG_CREATE_NORMAL_MAPS
	vec3d				cl_act_n = ( *m_pvvclNormals )[ uiLoop ][ cui_size - 1 ];
 #endif*/
#endif
	unsigned int		ui_vertex = 0;
	vec2d				cl_next;

//    std::cerr.precision( 16 );
//	std::cerr << " - " << cl_act << std::endl;

	while( ui_vertex < cui_size )
	{
		cl_next = ( *pvccrd )[ uiLoop ][ ui_vertex ];
/*		if( cl_next.x < m_clMin.x ) cl_next.x = m_clMin.x;
		else if( cl_next.x > m_clMax.x ) cl_next.x = m_clMax.x;
		if( cl_next.y < m_clMin.y ) cl_next.y = m_clMin.y;
		else if( cl_next.y > m_clMax.y ) cl_next.y = m_clMax.y;*/
//		std::cerr << cl_act << " -> " << cl_next << std::endl;
#ifdef OSG_FORCE_NO_T_VERTICES
// #ifdef OSG_CREATE_NORMAL_MAPS
		if( !StateTransition2( cl_act, cl_next, el,
							   cl_act_s, ( *m_pvvclSewed )[ uiLoop ][ ui_vertex ] ) )
/* #else
		if( !StateTransition2( cl_act, cl_next, el,
							   cl_act_s, ( *m_pvvclSewed )[ uiLoop ][ ui_vertex ],
							   cl_act_n, ( *m_pvvclNormals )[ uiLoop ][ ui_vertex ] ) )
 #endif*/
#else
		if( !StateTransition2( cl_act, cl_next, el,
							   cl_act_s, ( *m_pvvclSewed )[ uiLoop ][ ui_vertex ] ) )
#endif
		{
			cl_act = ( *pvccrd )[ uiLoop ][ ui_vertex ];
			cl_act_s = ( *m_pvvclSewed )[ uiLoop ][ ui_vertex ];
#ifdef OSG_FORCE_NO_T_VERTICES
/* #ifndef OSG_CREATE_NORMAL_MAPS
			cl_act_n = ( *m_pvvclNormals )[ uiLoop ][ ui_vertex ];
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
			++m_uiPosCnt;
 #endif
#endif
			++ui_vertex;
//			std::cerr << " - " << cl_act << std::endl;
		}
	}
}

int ParSpaceTrimmer::PerformTrimming( void )
{
	if( tcs3d == NULL )
	{
		// no 3d trimming curves :-(
#ifdef OSG_USE_SIMPLIFIER
		m_pclQuadTree->error_epsilon *= OSG_SIMPLIFIER_TESS_ERR;
		terr = m_pclQuadTree->error_epsilon * 3;
#endif
//		std::cerr << std::endl;
		bezier2ddequevector::iterator tcs_end = tcs->end();
		vcel.resize( tcs->size( ) );
//		std::cerr << tcs->size( ) << " trimming loops" << std::endl;
		unsigned int i = 0;
		for( bezier2ddequevector::iterator bcq = tcs->begin( ); bcq != tcs_end; ++bcq )
		{
			if( !( *bcq ).empty( ) )
			{
				vcel[ i ].clear( );
	            initializeStartState( *bcq );
	            processCurve( *bcq, vcel[ i ] );
//	              mergeCurve();
				++i;
			}
	    }
		if( i != vcel.size( ) )
		{
			vcel.resize( i );
		}
//		std::cerr << "vcel.size()::: " << vcel.size( );
	}
	else
	{
		// ok, we have 3d trimming curves :-)
		std::vector< double > params;
		
#ifdef OSG_USE_SIMPLIFIER
		terr *= OSG_SIMPLIFIER_TESS_ERR;
#endif
		bezier2ddequevector::iterator tcs_end = tcs->end();
		vcel.resize( tcs->size( ) );
//		std::cerr << tcs->size( ) << " trimming loops" << std::endl;
		unsigned int i = 0;
		int err;
		const unsigned int lc = tcs->size( );
//		bezier3ddequevector::iterator bcq3d = tcs3d->begin( );
//		bezier2ddequevector::iterator bcq = tcs->begin( );
		for( unsigned int l = 0; l < lc; ++l )
		{
			if( !( *tcs )[ l ].empty( ) )
			{
				vcel[ i ].clear( );
				const unsigned int cc = ( *tcs )[ l ].size( );
				for( unsigned int c = 0; c < cc; ++c )
				{
//					std::cerr << ".";
					BezierCurve2D &bc = ( *tcs )[ l ][ c ];
					BezierCurve3D bc3d = ( *tcs3d )[ l ][ c ];
//					bc.write( );
//					bc3d.write( );
//					std::cerr << std::endl;
					params.clear( );
					// MIDPOINT_SUBDIVISION / ARBITRARY_SUBDIVISION, POINT_DISTANCE / LINE_DISTANCE
					bc3d.approximate( params, terr, BezierCurve3D::MIDPOINT_SUBDIVISION | BezierCurve3D::LINE_DISTANCE );
					// leave out last point since it is the start of the next curve
					for( unsigned int j = 0; j < params.size( ); ++j )
					{
						vec2d	cl_pos;

						err = 0;
						cl_pos = bc.computewdeCasteljau( params[ j ], err );
						cl_pos.x = DCTP_EPS * 100.0 * floor( cl_pos.x / ( DCTP_EPS * 100.0 ) + 0.5 );
						cl_pos.y = DCTP_EPS * 100.0 * floor( cl_pos.y / ( DCTP_EPS * 100.0 ) + 0.5 );
						vcel[ i ].push_back( mesh->AddVertex( cl_pos ) );
//						std::cerr << bc.computewdeCasteljau( params[ j ], err ) << " ";
					}
//					std::cerr << std::endl;
				}
				++i;
			}
	    }
//		std::cerr << "x";
		if( i != vcel.size( ) )
		{
			vcel.resize( i );
		}
//		std::cerr << "vcel.size()::: " << vcel.size( );
		tcs = NULL;
		tcs3d = NULL;
	}
	// check and fix edge loops...
#ifdef OSG_USE_SIMPLIFIER
	unsigned int				ui_vert_cnt;
	unsigned int				ui_vert;
	int							i_err;
	const double				cd_error_quad = terr * terr
											  * ( 1.0 - OSG_SIMPLIFIER_TESS_ERR ) * ( 1.0 - OSG_SIMPLIFIER_TESS_ERR )
											  / ( OSG_SIMPLIFIER_TESS_ERR * OSG_SIMPLIFIER_TESS_ERR );
//	const double				cd_error_quad = DBL_MAX;
	double						d_act_dist;
	double						d_max_dist;
	unsigned int				ui_new;

	std::vector< SPolySimVertex >	vt_vertices;
	SPolySimVertexSet			vt_sim_sort;
	unsigned int				ui_first;
	unsigned int				ui_prev;
	unsigned int				ui_next;
	vec2d						cl_uv;
	unsigned int				ui_remain_cnt;
	unsigned int				i;
	vec3d						cl_midpos;

/*	for( i = 0; i < vcel.size( ); ++i )
	{
		ui_vert_cnt = vcel[ i ].size( );
		if( ui_vert_cnt )
		{
			for( ui_vert = 0; ui_vert < ui_vert_cnt; ++ui_vert )
			{
				cl_uv.x = vcel[ i ][ ui_vert ]->coords.x;
				cl_uv.y = vcel[ i ][ ui_vert ]->coords.y;
				std::cerr << cl_uv << m_pclNurbs->compute( cl_uv, i_err ) << std::endl;
			}
			std::cerr << std::endl;
		}
	}*/

	checkEdgeloops( );

	for( i = 0; i < vcel.size( ); ++i )
	{
		ui_vert_cnt = vcel[ i ].size( );
//		std::cerr << ui_vert_cnt << " -> ";
		if( ui_vert_cnt )
		{
			vt_vertices.resize( ui_vert_cnt );
			for( ui_vert = 0; ui_vert < ui_vert_cnt; ++ui_vert )
			{
				i_err = 0;
				vt_vertices[ ui_vert ].uiIndex = ui_vert;
				vt_vertices[ ui_vert ].uiPrev = ui_vert - 1;
				vt_vertices[ ui_vert ].uiNext = ui_vert + 1;
				cl_uv.x = vcel[ i ][ ui_vert ]->coords.x;
				cl_uv.y = vcel[ i ][ ui_vert ]->coords.y;
				vt_vertices[ ui_vert ].clPos = m_pclNurbs->compute( cl_uv, i_err );
				if( i_err )
				{
					std::cerr << " " << i_err << std::endl;
				}
			}
			vt_vertices[ 0 ].uiPrev = ui_vert_cnt - 1;
			vt_vertices[ ui_vert_cnt - 1 ].uiNext = 0;
			vt_sim_sort.clear( );
			for( ui_vert = 0; ui_vert < ui_vert_cnt; ++ui_vert )
			{
				cl_uv.x = ( vcel[ i ][ vt_vertices[ ui_vert ].uiPrev ]->coords.x
						  + vcel[ i ][ vt_vertices[ ui_vert ].uiNext ]->coords.x ) * 0.5;
				cl_uv.y = ( vcel[ i ][ vt_vertices[ ui_vert ].uiPrev ]->coords.y
						  + vcel[ i ][ vt_vertices[ ui_vert ].uiNext ]->coords.y ) * 0.5;
				cl_midpos = m_pclNurbs->compute( cl_uv, i_err );
				vt_vertices[ ui_vert ].dSimplifyError =
					osgMax( DistToEdge( vt_vertices[ ui_vert ].clPos,
										vt_vertices[ vt_vertices[ ui_vert ].uiPrev ].clPos,
										vt_vertices[ vt_vertices[ ui_vert ].uiNext ].clPos ),
							DistToEdge( cl_midpos,
										vt_vertices[ vt_vertices[ ui_vert ].uiPrev ].clPos,
										vt_vertices[ vt_vertices[ ui_vert ].uiNext ].clPos ) );
				vt_vertices[ ui_vert ].dSimplifyError +=
					sqrt( ( vt_vertices[ vt_vertices[ ui_vert ].uiPrev ].clPos
						  - vt_vertices[ vt_vertices[ ui_vert ].uiNext ].clPos ).quad_size( ) ) * 0.00001;
				if( vt_vertices[ ui_vert ].dSimplifyError < cd_error_quad )
				{
					vt_sim_sort.insert( &vt_vertices[ ui_vert ] );
				}
			}

			ui_first = 0;
			ui_remain_cnt = ui_vert_cnt;
			while( ( !vt_sim_sort.empty( ) ) && ( ui_remain_cnt > 3 ) )
			{
				// remove vertex
				ui_vert = ( *vt_sim_sort.begin( ) )->uiIndex;
				ui_prev = ( *vt_sim_sort.begin( ) )->uiPrev;
				ui_next = ( *vt_sim_sort.begin( ) )->uiNext;
//				std::cerr << "remove " << ui_vert << " next: " << ui_next << " prev: " << ui_prev << " err: " << sqrt( ( *vt_sim_sort.begin( ) )->dSimplifyError ) << std::endl;
				vt_sim_sort.erase( vt_sim_sort.begin( ) );
				vt_vertices[ ui_prev ].uiNext = ui_next;
				vt_vertices[ ui_next ].uiPrev = ui_prev;
				if( ui_vert == ui_first )
				{
					ui_first = ui_next;
				}
				if( !mesh->findVertex( vcel[ i ][ ui_vert ]->coords ) )
				{
//					std::cerr << "delete vertex4:" << ui_vert << " " << ( void* ) vcel[ i ][ ui_vert ] << std::endl;
					delete vcel[ i ][ ui_vert ];
					vcel[ i ][ ui_vert ] = NULL;
				}
				// remove prev and next from qeue
				if( vt_vertices[ ui_prev ].dSimplifyError < cd_error_quad )
				{
					vt_sim_sort.erase( &vt_vertices[ ui_prev ] );
				}
				if( vt_vertices[ ui_next ].dSimplifyError < cd_error_quad )
				{
					vt_sim_sort.erase( &vt_vertices[ ui_next ] );
				}
				// compute new errors
				if( ( ui_vert != ui_prev ) && ( ui_vert != ui_next ) )
				{
					cl_uv.x = ( vcel[ i ][ vt_vertices[ ui_prev ].uiPrev ]->coords.x
							  + vcel[ i ][ ui_next ]->coords.x ) * 0.5;
					cl_uv.y = ( vcel[ i ][ vt_vertices[ ui_prev ].uiPrev ]->coords.y
							  + vcel[ i ][ ui_next ]->coords.y ) * 0.5;
					cl_midpos = m_pclNurbs->compute( cl_uv, i_err );
					d_max_dist = DistToEdge( cl_midpos,
											 vt_vertices[ vt_vertices[ ui_vert ].uiPrev ].clPos,
											 vt_vertices[ ui_next ].clPos );

					ui_vert = ( vt_vertices[ ui_prev ].uiPrev + 1 ) % ui_vert_cnt;
					while( ui_vert != ui_next )
					{
						d_act_dist = DistToEdge( vt_vertices[ ui_vert ].clPos,
												 vt_vertices[ vt_vertices[ ui_prev ].uiPrev ].clPos,
												 vt_vertices[ ui_next ].clPos );
						if( d_act_dist > d_max_dist )
						{
							d_max_dist = d_act_dist;
						}
						ui_vert = ( ui_vert + 1 ) % ui_vert_cnt;
					}
					vt_vertices[ ui_prev ].dSimplifyError = d_max_dist;
					vt_vertices[ ui_prev ].dSimplifyError +=
						sqrt( ( vt_vertices[ vt_vertices[ ui_prev ].uiPrev ].clPos
							  - vt_vertices[ vt_vertices[ ui_prev ].uiNext ].clPos ).quad_size( ) ) * 0.00001;
					if( vt_vertices[ ui_prev ].dSimplifyError < cd_error_quad )
					{
						vt_sim_sort.insert( &vt_vertices[ ui_prev ] );
					}

					cl_uv.x = ( vcel[ i ][ ui_prev ]->coords.x
							  + vcel[ i ][ vt_vertices[ ui_next ].uiNext ]->coords.x ) * 0.5;
					cl_uv.y = ( vcel[ i ][ ui_prev ]->coords.y
							  + vcel[ i ][ vt_vertices[ ui_next ].uiNext ]->coords.y ) * 0.5;
					cl_midpos = m_pclNurbs->compute( cl_uv, i_err );
					d_max_dist = DistToEdge( cl_midpos,
											 vt_vertices[ ui_prev ].clPos,
											 vt_vertices[ vt_vertices[ ui_next ].uiNext ].clPos );

					ui_vert = ( ui_prev + 1 ) % ui_vert_cnt;
					while( ui_vert != vt_vertices[ ui_next ].uiNext )
					{
						d_act_dist = DistToEdge( vt_vertices[ ui_vert ].clPos,
												 vt_vertices[ ui_prev ].clPos,
												 vt_vertices[ vt_vertices[ ui_next ].uiNext ].clPos );
						if( d_act_dist > d_max_dist )
						{
							d_max_dist = d_act_dist;
						}
						ui_vert = ( ui_vert + 1 ) % ui_vert_cnt;
					}
					vt_vertices[ ui_next ].dSimplifyError = d_max_dist;
					vt_vertices[ ui_next ].dSimplifyError +=
						sqrt( ( vt_vertices[ vt_vertices[ ui_next ].uiPrev ].clPos
							  - vt_vertices[ vt_vertices[ ui_next ].uiNext ].clPos ).quad_size( ) ) * 0.00001;
					if( vt_vertices[ ui_next ].dSimplifyError < cd_error_quad )
					{
						vt_sim_sort.insert( &vt_vertices[ ui_next ] );
					}
				}
				--ui_remain_cnt;
			}

			// setup new loop
			vcel[ i ][ 0 ] = vcel[ i ][ ui_first ];
			ui_new = 1;
			for( ui_vert = vt_vertices[ ui_first ].uiNext; ui_vert != ui_first; ui_vert = vt_vertices[ ui_vert ].uiNext )
			{
				vcel[ i ][ ui_new ] = vcel[ i ][ ui_vert ];
				++ui_new;
			}
			vcel[ i ].resize( ui_new );
//			std::cerr << "simplified " << ui_vert_cnt << " to " << ui_new << std::endl;
		}

//		std::cerr << vcel[ i ].size( ) << std::endl;
	}

//	vpcl_new.clear( );

	vt_vertices.clear( );
	vt_sim_sort.clear( );
#endif
	checkEdgeloops( );
//	std::cerr << " -> " << vcel.size( ) << std::endl;

	// copy edge loops in array
	pvccrd->resize( vcel.size( ) );
	for( unsigned int loop = 0; loop < vcel.size( ); ++loop )
	{
		unsigned int	v, /*eid,*/ oid;
		unsigned int	nid = vcel[ loop ][ vcel[ loop ].size( ) - 1 ]->id;
		vec2d			cl_temp_vec;

		for( v = 0; v < vcel[ loop ].size( ); ++v )
		{
//			std::cerr << "edge " << v << std::endl;
			oid = nid;
			nid = vcel[ loop ][ v ]->id;
			if( oid != nid )
			{
				cl_temp_vec.x = vcel[ loop ][ v ]->coords.x;
				cl_temp_vec.y = vcel[ loop ][ v ]->coords.y;
//				std::cerr << "oid " << oid << ", nid " << nid << std::endl;
//				std::cerr << "tvec " << cl_temp_vec << std::endl;
				( *pvccrd )[ loop ].push_back( cl_temp_vec );
			}
		}
		// close loop
		( *pvccrd )[ loop ].push_back( ( *pvccrd )[ loop ][ 0 ] );
	}
//        std::cerr <<"outta here" << std::endl;
	return 0;
}

int ParSpaceTrimmer::PerformTrimming2( void )
{
	unsigned int	ui_tloops = ( *pvccrd ).size( );
	unsigned int	ui_i;

	m_bDeleteVertexInfo = true;	// vertexinfo holds vec3d*
	vcel.resize( ui_tloops );
//	m_vbReversed.resize( ui_tloops );
//	std::cerr << ui_tloops << " trimming loops" << std::endl;
#ifdef OSG_KEEP_2D_POINTS
	m_uiPosCnt = 0;
#endif

#ifdef OSG_SPLIT_HOLE_CELLS
	// split cells containing a hole
	for( ui_i = 0; ui_i < ui_tloops; ++ui_i )
	{
		if( ( ( *m_pvbUsed )[ ui_i ] ) &&
			( ( *m_pvbReversed )[ ui_i ] ) )
		{
			unsigned int	v; //, eid, oid;
			vec2d			cl_temp_vec;
			DCTPFace		*pcl_inside_face;
			vec2d			cl_min;
			vec2d			cl_max;
			double			d_ratio;

			cl_temp_vec.x = ( *pvccrd )[ ui_i ][ ( *pvccrd )[ ui_i ].size( ) - 1 ].x;
			cl_temp_vec.y = ( *pvccrd )[ ui_i ][ ( *pvccrd )[ ui_i ].size( ) - 1 ].y;
			cl_min = cl_max = cl_temp_vec;

			getStartingFace( cl_temp_vec );
			pcl_inside_face = state.f;
			for( v = 0; v < ( *pvccrd )[ ui_i ].size( ); ++v )
			{
				if( pcl_inside_face )
				{
					cl_temp_vec.x = ( *pvccrd )[ ui_i ][ v ].x;
					cl_temp_vec.y = ( *pvccrd )[ ui_i ][ v ].y;
					cl_min.x = osgMin( cl_min.x, cl_temp_vec.x );
					cl_min.y = osgMin( cl_min.y, cl_temp_vec.y );
					cl_max.x = osgMax( cl_max.x, cl_temp_vec.x );
					cl_max.y = osgMax( cl_max.y, cl_temp_vec.y );
					if( !isOverFace( pcl_inside_face, cl_temp_vec ) )
					{
						pcl_inside_face = NULL;
					}
				}
			}
			if( pcl_inside_face )
			{
				if( cl_max.x - cl_min.x > cl_max.y - cl_min.y )
				{
					d_ratio = ( ( cl_min.x + cl_max.x ) * 0.5 - pcl_inside_face->orig_face[ 0 ]->coords.x )
							/ ( pcl_inside_face->orig_face[ 1 ]->coords.x
							  - pcl_inside_face->orig_face[ 0 ]->coords.x );

//					std::cerr << "x: " << d_ratio << std::endl;

					mesh->SubdivideQuadEW( pcl_inside_face, 1.0 - d_ratio );
				}
				else
				{
					d_ratio = ( ( cl_min.y + cl_max.y ) * 0.5 - pcl_inside_face->orig_face[ 3 ]->coords.y )
							/ ( pcl_inside_face->orig_face[ 0 ]->coords.y
							  - pcl_inside_face->orig_face[ 3 ]->coords.y );

//					std::cerr << "y: " << d_ratio << std::endl;

					mesh->SubdivideQuadNS( pcl_inside_face, 1.0 - d_ratio );
				}
			}
		}
	}
#endif

	for( ui_i = 0; ui_i < ui_tloops; ++ui_i )
	{
		if( ( *m_pvbUsed )[ ui_i ] )
		{
//			std::cerr << "new curve " << ( *pvccrd )[ ui_i ].size( ) << std::endl;
#ifdef OSG_KEEP_2D_POINTS
			m_uiPosCnt += ( *pvccrd )[ ui_i ].size( );
#endif
			if( isLoopValid( ui_i ) )
			{
#ifdef OSG_KEEP_2D_POINTS
				--m_uiPosCnt;
#endif
				initializeStartState2( ui_i, vcel[ ui_i ] );
#ifdef OSG_KEEP_2D_POINTS
				m_uiPosCnt -= ( *pvccrd )[ ui_i ].size( );
				++m_uiPosCnt;
#endif
				processCurve2( ui_i, vcel[ ui_i ] );
			}
		}
	}

	for( ui_i = 0; ui_i < ui_tloops; ++ui_i )
	{
		if( vcel[ ui_i ].size( ) == 0 )
		{
//			std::cerr << "deleting empty trimming loop" << std::endl;
			--ui_tloops;
			vcel[ ui_i ] = vcel[ ui_tloops ];
			vcel.pop_back( );
			--ui_i;
		}
	}
//	std::cerr << "+++ " << m_uiPosCnt << " +++" << std::endl;

	return 0;
}

vec2d ParSpaceTrimmer::calcNormal( vec2d &a, vec2d &b )
{
  vec2d norm(0.0, 0.0);
  if ( a == b ) return norm;
  double lab = sqrt( ( a.x - b.x ) * ( a.x - b.x ) + ( a.y - b.y ) * ( a.y - b.y ) );
  norm.x = -( b.y - a.y ) / lab; // This is a normal -> rotated 90 degrees
  norm.y = ( b.x - a.x ) / lab;
  return norm;
}

bool ParSpaceTrimmer::checkEdgeNormals(DirectedGraph< vec2d, unsigned char > &sg , int from, int to )
{
  if ( sg.nodes[ from ].nodeinfo == sg.nodes[ to ].nodeinfo ) return false;
  vec2d norm = calcNormal( sg.nodes[ from ].nodeinfo, sg.nodes[ to ].nodeinfo );

  //loop through all the edges in both nodes and check if the normal of the
  //non-directed edges are not the same
  for( unsigned int i = 0; i < sg.nodes[ from ].edges.size(); ++i ) {
    vec2d a = sg.nodes[ sg.edges[ sg.nodes[ from ].edges[ i ]].from ].nodeinfo;
    vec2d b = sg.nodes[ sg.edges[ sg.nodes[ from ].edges[ i ]].to ].nodeinfo;
    vec2d n = calcNormal( a, b );
///    std::cerr <<"n: " << n << " norm: " << norm << std::endl;
///    std::cerr << "v1: " << sg.nodes[ from ].nodeinfo << " v2: " <<
///         sg.nodes[ to ].nodeinfo << std::endl;
    if ( n == norm ) {
///      std::cerr << "same normal found..." << std::endl;
///      std::cerr << "v1: " << sg.nodes[ from ].nodeinfo << " v2: " <<
///           sg.nodes[ to ].nodeinfo << std::endl;
      return true;
    }
    //if ( !sg.edges[ sg.nodes[ nodeidx ].edges[ i ] ].direction ) return true;
  }
  return false;
}

/*void ParSpaceTrimmer::insertBezierApproximation( BezierCurve2D& b,
                                                 int& start_node,
                                                 DCTPFace *face,
                                                 bool islast,
                                                 DirectedGraph< vec2d, unsigned char > &sg ) {
        double &norm = face->norm;
        vec2dvector approx;
        unsigned char dummy = 0x00;
        b.approximate( approx, norm );
        vec2dvector::iterator ae = approx.end();
        if ( approx.size() == 2 ) {
          if ( fabs( approx[ 0 ].x - approx[ 1 ].x ) < DCTP_EPS &&
               ( fabs (approx[0].x - face->orig_quad[0]->coords.x ) < DCTP_EPS ||
                 fabs (approx[0].x - face->orig_quad[1]->coords.x ) < DCTP_EPS ||
                 fabs (approx[0].x - face->orig_quad[2]->coords.x ) < DCTP_EPS ||
                 fabs (approx[0].x - face->orig_quad[3]->coords.x ) < DCTP_EPS)) {
//              std::cerr <<"we gonna be in x trouble..." << islast << std::endl;
              //now insert a dummy node at (0.5) and two linear approx. edges
              //to this node. This will work, because the Bezier curve
              //can't intersect the current face only at its endpoints
              //otherwise it would lie on the face.
              //FIXME: a nicer solution would be to go through the relevant
              //FIXME: edges in the graph between the endpoints of this curve
              //FIXME: (there must be edges like this) and orient them properly
              int err;
              vec2d v;
              v = b.computewdeCasteljau( 0.5, err );
              if ( err ) { std::cerr << "unexpected error..." << std::endl; return; }
//              if ( fabs( v.x - approx[0].x ) < DCTP_EPS ) std::cerr << "life suxx..." << std::endl;
//              std::cerr << "lofaszpicsa: " << fabs( v.x - approx[0].x ) << std::endl;
//              std::cerr << "v1: " << approx[0] << " v2: " << approx[1] << std::endl;
//              dumpcontrolpoints( b );
              int nid = sg.AddNode( v );
//              std::cerr << "inserting lofasz: " << v << std::endl;
//			  std::cerr << "inserting edge " << start_node << ", " << nid << std::endl;
              sg.AddEdge( dummy, start_node, nid, true );
//              std::cerr <<"gecicoords: " << sg.nodes[ nid ].nodeinfo << std::endl;
              start_node = nid;
              return;
          }
          if ( fabs( approx[ 0 ].y - approx[ 1 ].y ) < DCTP_EPS &&
               ( fabs (approx[0].y - face->orig_quad[0]->coords.y ) < DCTP_EPS ||
                 fabs (approx[0].y - face->orig_quad[1]->coords.y ) < DCTP_EPS ||
                 fabs (approx[0].y - face->orig_quad[2]->coords.y ) < DCTP_EPS ||
                 fabs (approx[0].y - face->orig_quad[3]->coords.y ) < DCTP_EPS)) {
//              std::cerr <<"we gonna be in y trouble..." << islast << std::endl;
              int err;
              vec2d v;
              v = b.computewdeCasteljau( 0.5, err );
              if ( err ) { std::cerr << "unexpected error..." << std::endl; return; }
              int nid = sg.AddNode( v );
//			  std::cerr << "inserting edge " << start_node << ", " << nid << std::endl;
              sg.AddEdge( dummy, start_node, nid, true );
              start_node = nid;
              return;

          }
        } //approx size
//DEBUG
//        int zz = 0;
//        for( vec2dvector::iterator i = approx.begin(); i != ae; ++i ) {
//          std::cerr << "zz: " << zz++ << " *i: " << *i << std::endl;
//        }
//        std::cerr <<"std::endlofaszpicsagecisize():"<< approx.size() << std::endl;
//DEBUG END
//DEBUG
//        ofstream outfile;
//        outfile.open( "beziers.dat", ios::app );
//        plotbezier( b, outfile );
//        outfile.close();
//DEBUG END
        if ( islast ) ae--;
//        zz = 0;
        for( vec2dvector::iterator i = approx.begin() + 1; i != ae; ++i ) {
//DEBUG
//                vec2dvector::iterator k;
//                zz++;
//                if ( i != approx.begin() ) {
//                  k = i - 1;
//                  if ( *i == *k ) std::cerr <<"FUCK FUCK FUCK FUCK FUCK " << zz << " " << *i << std::endl;
//                }
//DEBUG END
                int nid = sg.AddNode( *i );
//                if ( !checkEdgeNormals( sg, start_node, nid ))
//				std::cerr << "inserting edge " << start_node << ", " << nid << std::endl;
                sg.AddEdge( dummy, start_node, nid, true );
		start_node = nid;
	}
}*/

#ifdef OSG_FORCE_NO_T_VERTICES
 #ifdef OSG_KEEP_2D_POINTS
int ParSpaceTrimmer::buildSurfaceGraph( DirectedGraph< vec2d, unsigned char > *psg, std::vector< vec3d > *pvclSewed, std::vector< vec3d > *pvclNormals, std::vector< unsigned int > *pvuiIdx )
 #else
int ParSpaceTrimmer::buildSurfaceGraph( DirectedGraph< vec2d, unsigned char > *psg, std::vector< vec3d > *pvclSewed, std::vector< vec3d > *pvclNormals )
 #endif
#else
int ParSpaceTrimmer::buildSurfaceGraph( DirectedGraph< vec2d, unsigned char > *psg, std::vector< vec3d > *pvclSewed )
#endif
{
	dctpvertexset::iterator ve = mesh->vertices.end();
	dctpvertexset::iterator i;
	std::vector< DCTPVertex* >	vpcl_added_from;
	std::vector< DCTPVertex* >	vpcl_added_to;

	// copy all sewed points with 3d pos
#ifdef OSG_FORCE_NO_T_VERTICES
	if( pvclNormals ) pvclNormals->clear( );
#endif
#ifdef OSG_KEEP_2D_POINTS
	if( pvuiIdx ) pvuiIdx->clear( );
#endif
	if( pvclSewed ) pvclSewed->clear( );
	psg->nodes.clear( );
	psg->edges.clear( );
	for( i = mesh->vertices.begin(); i != ve; ++i )
	{
		if( ( *i )->vertexinfo )
		{
			vec2d v2dtemp( ( *i )->coords.x, ( *i )->coords.y );
//			std::cerr << "coords: " << v2dtemp << std::endl;
			(*i)->node_id = psg->AddNode( v2dtemp );
//			std::cerr << ( vec3d* ) ( *i )->vertexinfo;
//			std::cerr << "sewed: " << *( ( vec3d* ) ( *i )->vertexinfo ) << std::endl;
// 			std::cerr << "ids: " << pvclSewed->size( ) << " " << ( *i )->node_id << std::endl;
#ifdef OSG_FORCE_NO_T_VERTICES
			if( pvclSewed ) pvclSewed->push_back( ( ( SPosNorm* ) ( *i )->vertexinfo )->clPos );
/* #ifndef OSG_CREATE_NORMAL_MAPS
			if( pvclNormals ) pvclNormals->push_back( ( ( SPosNorm* ) ( *i )->vertexinfo )->clNorm );
 #endif*/
 #ifdef OSG_KEEP_2D_POINTS
//			std::cerr << pvuiIdx->size( ) << ": " << ( ( SPosNorm* ) ( *i )->vertexinfo )->uiNum << std::endl;
			if( pvuiIdx ) pvuiIdx->push_back( ( ( SPosNorm* ) ( *i )->vertexinfo )->uiNum );
 #endif
#else
			if( pvclSewed ) pvclSewed->push_back( *( ( vec3d* ) ( *i )->vertexinfo ) );
#endif
		}
	}
#ifdef OSG_KEEP_2D_POINTS
	// add empty entries to have space for trimming loop vertices
	if( pvclSewed->size( ) < m_uiPosCnt )
	{
		vec2d				cl_dummy( 0.0, 0.0 );
		const unsigned int	cui_idx_cnt = m_uiPosCnt - pvclSewed->size( );

		for( unsigned int ui_idx = 0; ui_idx < cui_idx_cnt; ++ui_idx )
		{
			psg->AddNode( cl_dummy );
		}
		if( pvclSewed ) pvclSewed->resize( m_uiPosCnt );
		if( pvclNormals ) pvclNormals->resize( m_uiPosCnt );
	}
#endif

	// copy all inner points without 3d pos
	for( i = mesh->vertices.begin(); i != ve; ++i )
	{
		if( ( *i )->vertexinfo == NULL )
		{
			vec2d v2dtemp( ( *i )->coords.x, ( *i )->coords.y );
//			std::cerr << "coords: " << v2dtemp << std::endl;
			(*i)->node_id = psg->AddNode( v2dtemp );
		}
	}

#ifdef OSG_NO_EDGE_SET
	dctpedgevector::iterator ee = mesh->edges.end();
#else
	dctpedgeset::iterator ee = mesh->edges.end();
#endif
	unsigned char dummy = 0;
	
#ifdef OSG_NO_EDGE_SET
	for( dctpedgevector::iterator e = mesh->edges.begin(); e != ee; ++e )
#else
	for( dctpedgeset::iterator e = mesh->edges.begin(); e != ee; ++e )
#endif
	{
		DCTPVertex *v1, *v2;
		bool orient;

		(*e)->getVertices( v1, v2 );
		if( (*e)->orientation < 0 )
		{
			DCTPVertex *tmpv;
			tmpv = v1;
			v1 = v2;
			v2 = tmpv;
			orient = true;
		}
		else if( (*e)->orientation > 0 )
		{
			orient = true;
		}
		else
		{
			orient = false;
		}

//		if( orient ) std::cerr << "adding edge " << v1->node_id << ", " << v2->node_id << std::endl;
		dummy = orient ? 1 : 0;	// set edgeinfo if edge belongs to trimming
		psg->AddEdge( dummy, v1->node_id, v2->node_id, orient );
		if( abs( (*e)->orientation ) > 1 )
		{
			for( int i_idx = 1; i_idx < abs( (*e)->orientation ); ++i_idx )
			{
				psg->AddEdge( dummy, v1->node_id, v2->node_id, orient );
			}
        }
	}
 /*       dctpfacevector::iterator fe = mesh->faces.end();
        for( dctpfacevector::iterator i = mesh->faces.begin();
             i != fe; ++i ) {
                //FIXME sometime - whole trimming curve inside polygon
                //is not yet handled
                std::vector< TrimSegment >::iterator te = (*i)->trimseg.end();
                for( std::vector< TrimSegment >::iterator j = (*i)->trimseg.begin();
                     j != te; ++j ) {
                        bezier2dvector::iterator ce = j->trimbeziers.end();
                        int sn = j->start->node_id; //start node id
                        for( bezier2dvector::iterator k =
                             j->trimbeziers.begin(); k != ce; ++k )
                                insertBezierApproximation( *k, sn,
                                                           *i,
                                                           ( k == ce - 1 ),
                                                           *psg );
                        //gecc'
                        if ( !(void*)j->end ) {
                          std::cerr << "Null end node..." << std::endl;
                          throw ParSpaceTrimmerError( ParSpaceTrimmerError::ERR_NULL_END_NODE );
                        }
///                        std::cerr << "end node: " << (void*) j->end;
///                        std::cerr << "  end node id: " << j->end->node_id;
//						std::cerr << "inserting end node " << sn << ", " << j->end->node_id << std::endl;
                        psg->AddEdge( dummy, sn, j->end->node_id, true );
                }//trimsegment cyc over face
        }//all face cyc*/

#ifndef OSG_SPLIT_HOLE_CELLS
	// add two edges for each hole in a single tree cell
	for( unsigned int loop = 0; loop < vcel.size( ); ++loop )
	{
//		std::cerr << "edge loop " << loop << " of " << vcel.size( ) << " (" << vcel[ loop ].size( ) << ")" << std::endl;
		unsigned int	v; //, eid, oid;
//		std::cerr << "adr:" << vcel[ loop ][ vcel[ loop ].size( ) - 1 ] << std::endl;
		unsigned int	nid = vcel[ loop ][ vcel[ loop ].size( ) - 1 ]->node_id;
		vec2d			cl_temp_vec;
		DCTPFace		*pcl_inside_face;

		cl_temp_vec.x = vcel[ loop ][ vcel[ loop ].size( ) - 1 ]->coords.x;
		cl_temp_vec.y = vcel[ loop ][ vcel[ loop ].size( ) - 1 ]->coords.y;
		if( ( *m_pvbReversed )[ loop ] )
		{
			getStartingFace( cl_temp_vec );
			pcl_inside_face = state.f;
			for( v = 0; v < vcel[ loop ].size( ); ++v )
			{
//				std::cerr << "edge " << v << std::endl;
//				oid = nid;
//				nid = vcel[ loop ][ v ]->node_id;
				if( pcl_inside_face )
				{
					cl_temp_vec.x = vcel[ loop ][ v ]->coords.x;
					cl_temp_vec.y = vcel[ loop ][ v ]->coords.y;
					if( vcel[ loop ][ vcel[ loop ].size( ) - 1 ]->faces.size( ) ) // intersects a face edge
//					if( !isOverFace( pcl_inside_face, cl_temp_vec ) )
					{
						pcl_inside_face = NULL;
					}
				}
//				if( oid != nid )
//				{
//					std::cerr << "oid " << oid << ", nid " << nid << std::endl;
//					std::cerr << "tvec " << tvec << std::endl;
//					std::cerr << "inserted in loop." << std::endl;
/*					eid = psg->FindEdge( oid, nid );
					if( eid == -1 )
					{
//						std::cerr << "adding edge to graph: from " << oid << " to " << nid << std::endl;
						psg->AddEdge( dummy, oid, nid, true );
					}
					else
					{
//						std::cerr << "directing edge in graph: from " << oid << " to " << nid << std::endl;
						psg->setEdgeDirection( eid, nid );
					}
				}*/
			}
			if( pcl_inside_face )
			{
				// loop is completely inside face
				// => find the nearest points to the upper left and lower right
				//    edge of the face which don't intersect the trimming loop
				//    and construct two undirected edges with these
				double			d_qdist_ul = -1.0;
				double			d_qdist_lr = -1.0;
				double			d_act_dist;
#ifdef OSG_UNION_TRI_QUAD
				DCTPVertex		*pcl_upper_left = pcl_inside_face->orig_face[ 0 ];
				DCTPVertex		*pcl_lower_right = pcl_inside_face->orig_face[ 2 ];
#else
				DCTPVertex		*pcl_upper_left = pcl_inside_face->orig_quad[ 0 ];
				DCTPVertex		*pcl_lower_right = pcl_inside_face->orig_quad[ 2 ];
#endif
				DCTPVertex		*pcl_nearest_ul;
				DCTPVertex		*pcl_nearest_lr;
				DCTPVertex		*pcl_act_point;
				unsigned int	ui_check;
				bool			b_ok;
				unsigned int	ui_exit;

				dummy = 1;	// edges don't belong to the tree

				for( v = 0; v < vcel[ loop ].size( ); ++v )
				{
					pcl_act_point = vcel[ loop ][ v ];
					d_act_dist = ( vec2d( pcl_act_point->coords - pcl_upper_left->coords ) ).quad_size( );
					if( ( d_qdist_ul < -0.5 ) || ( d_act_dist - d_qdist_ul < DCTP_EPS ) )
					{
						d_qdist_ul = d_act_dist;
						pcl_nearest_ul = pcl_act_point;
					}
					d_act_dist = ( vec2d( pcl_act_point->coords - pcl_lower_right->coords ) ).quad_size( );
					if( ( d_qdist_lr < -0.5 ) || ( d_act_dist - d_qdist_lr < DCTP_EPS ) )
					{
						d_qdist_lr = d_act_dist;
						pcl_nearest_lr = pcl_act_point;
					}
				}
				// check edge1
				b_ok = false;
				ui_exit = 1000;
				while( ( !b_ok ) && ( --ui_exit > 0 ) )
				{
//					std::cerr << "check " << pcl_nearest_ul->coords << " " << pcl_upper_left->coords << std::endl;
					b_ok = true;
					// check if there is an intersection with other loops!
					for( ui_check = 0; ui_check < vcel.size( ); ++ui_check )
					{
						if( ui_check != loop )
						{
							pcl_act_point = intersectsLoop( pcl_nearest_ul, pcl_upper_left, ui_check );
							if( pcl_act_point )
							{
								pcl_upper_left = pcl_act_point;
								ui_check = vcel.size( );
								b_ok = false;
								///std::cerr << "o=> " << pcl_nearest_ul->coords << " " << pcl_upper_left->coords << std::endl;
							}
						}
					}
					// check if there is an intersection with this loop!
					pcl_act_point = intersectsLoop( pcl_upper_left, pcl_nearest_ul, loop );
					if( pcl_act_point )
					{
						pcl_nearest_ul = pcl_act_point;
						b_ok = false;
						///std::cerr << "t=> " << pcl_nearest_ul->coords << " " << pcl_upper_left->coords << std::endl;
					}
					// check if there is an intersection with existing connect edges
					for( ui_check = 0; ui_check < vpcl_added_from.size( ); ++ui_check )
					{
						if( ( pcl_nearest_ul != vpcl_added_from[ ui_check ] ) &&
							( pcl_nearest_ul != vpcl_added_to[ ui_check ] ) &&
							( pcl_upper_left != vpcl_added_from[ ui_check ] ) &&
							( pcl_upper_left != vpcl_added_to[ ui_check ] ) )
						{
							if( doIntersect( vpcl_added_from[ ui_check ]->coords, vpcl_added_to[ ui_check ]->coords,
											 pcl_nearest_ul->coords, pcl_upper_left->coords, d_act_dist ) )
							{
								if( d_act_dist <= 1.0 )
								{
									if( d_act_dist < 0.5 )
									{
										pcl_upper_left = vpcl_added_from[ ui_check ];
									}
									else
									{
										pcl_upper_left = vpcl_added_to[ ui_check ];
									}
									b_ok = false;
									ui_check = vpcl_added_from.size( );
									///std::cerr << "e=> " << pcl_nearest_ul->coords << " " << pcl_upper_left->coords << std::endl;
								}
							}
						}
					}
					if( pcl_upper_left == pcl_nearest_ul )
					{
						b_ok = true;
					}
				}
				// save edge1
				if( pcl_upper_left != pcl_nearest_ul )
				{
					vpcl_added_from.push_back( pcl_upper_left );
					vpcl_added_to.push_back( pcl_nearest_ul );
					psg->AddEdge( dummy, pcl_upper_left->node_id,
								  pcl_nearest_ul->node_id, false );
				}
				///std::cerr << "ok 1" << std::endl;

				// check edge2
				b_ok = false;
				ui_exit = 1000;
				while( ( !b_ok ) && ( --ui_exit > 0 ) )
				{
//					std::cerr << "check " << pcl_nearest_lr->coords << " " << pcl_lower_right->coords << std::endl;
					b_ok = true;
					// check if there is an intersection with other loops!
					for( ui_check = 0; ui_check < vcel.size( ); ++ui_check )
					{
						if( ui_check != loop )
						{
							pcl_act_point = intersectsLoop( pcl_nearest_lr, pcl_lower_right, ui_check );
							if( pcl_act_point )
							{
								pcl_lower_right = pcl_act_point;
								ui_check = vcel.size( );
								b_ok = false;
								///std::cerr << "o=> " << pcl_nearest_lr->coords << " " << pcl_lower_right->coords << std::endl;
							}
						}
					}
					// check if there is an intersection with this loop!
					pcl_act_point = intersectsLoop( pcl_lower_right, pcl_nearest_lr, loop );
					if( pcl_act_point )
					{
						pcl_nearest_lr = pcl_act_point;
						b_ok = false;
						///std::cerr << "t=> " << pcl_nearest_lr->coords << " " << pcl_lower_right->coords << std::endl;
					}
					// check if there is an intersection with existing connect edges
					for( ui_check = 0; ui_check < vpcl_added_from.size( ); ++ui_check )
					{
						if( ( pcl_nearest_lr != vpcl_added_from[ ui_check ] ) &&
							( pcl_nearest_lr != vpcl_added_to[ ui_check ] ) &&
							( pcl_lower_right != vpcl_added_from[ ui_check ] ) &&
							( pcl_lower_right != vpcl_added_to[ ui_check ] ) )
						{
							if( doIntersect( vpcl_added_from[ ui_check ]->coords, vpcl_added_to[ ui_check ]->coords,
											 pcl_nearest_lr->coords, pcl_lower_right->coords, d_act_dist ) )
							{
								if( d_act_dist <= 1.0 )
								{
									if( d_act_dist < 0.5 )
									{
										pcl_lower_right = vpcl_added_from[ ui_check ];
									}
									else
									{
										pcl_lower_right = vpcl_added_to[ ui_check ];
									}
									b_ok = false;
									ui_check = vpcl_added_from.size( );
									///std::cerr << "e=> " << pcl_nearest_lr->coords << " " << pcl_lower_right->coords << std::endl;
								}
							}
						}
					}
					if( pcl_lower_right == pcl_nearest_lr )
					{
						b_ok = true;
					}
				}
				// save edge2
				if( pcl_lower_right != pcl_nearest_lr )
				{
					vpcl_added_from.push_back( pcl_lower_right );
					vpcl_added_to.push_back( pcl_nearest_lr );
					psg->AddEdge( dummy, pcl_lower_right->node_id,
								  pcl_nearest_lr->node_id, false );
				}
				///std::cerr << "ok 2" << std::endl;
			}
		}
	}
#endif
	return 0;
}

#ifndef OSG_FORCE_NO_T_VERTICES
void ParSpaceTrimmer::getTrimmingLoops( std::vector< std::vector< vec2d > > &rvvclTrimmingLoops )
{
	rvvclTrimmingLoops.resize( vcel.size( ) );
	for( unsigned int loop = 0; loop < vcel.size( ); ++loop )
	{
		rvvclTrimmingLoops[ loop ].resize( vcel[ loop ].size( ) - 1 );
		for( unsigned int v = 0; v < vcel[ loop ].size( ) - 1; ++v )
		{
			rvvclTrimmingLoops[ loop ][ v ] = vcel[ loop ][ v ]->coords;
		}
	}
}
#endif

int ParSpaceTrimmer::writeSurfaceGraph( std::ofstream &of )
{
	DirectedGraph< vec2d, unsigned char > sg; //surface_graph
	int err = buildSurfaceGraph( &sg );
	sg.write( of );
	return 0;
}

void ParSpaceTrimmer::writeInvalidSurfaceGraph( std::ofstream &of )
{
  DirectedGraph< vec2d, unsigned char > sg;
  sg.writeInvalid( of );
}

void ParSpaceTrimmer::checkEdgeloops( )
{
	std::vector< SScanLineEdge* >		vpt_start;
	std::vector< SScanLineEdge* >		vpt_end;
//	std::vector< SScanLineEdge* >		vpt_edges;
	ScanLineEventSet				spt_events;
	unsigned int					ui_loop;
	unsigned int					ui_vertex;
	unsigned int					ui_last;
	unsigned int					ui_size;
	unsigned int					ui_loop2;
	SScanLineEdge					*pt_act_edge;
	SScanLineEntry					*pt_scan_line = NULL;
	SScanLineEntry					*pt_act_entry;
	SScanLineEntry					*pt_next_entry;
	ScanLineEventSet::iterator		ispt_act_event;
	SScanLineEvent					*pt_act_event;
	int								i_inside;
	bool							b_valid;
	vec3d							cl_p;
	vec3d							cl_pp;

	// create scan line edges and sort them
	for( ui_loop = 0; ui_loop < vcel.size( ); ++ui_loop )
	{
		ui_size = vcel[ ui_loop ].size( );
//		ui_last = ui_size - 1;
		cl_p = vcel[ ui_loop ][ ui_size - 1 ]->coords;

		unsigned int			ui_new_num = 0;
		std::vector< DCTPVertex* >	vpcl_new_points( ui_size ); // waste some memory to have linear time

		for( ui_vertex = 0; ui_vertex < ui_size; ++ui_vertex )
		{
			cl_pp = cl_p;
			cl_p = vcel[ ui_loop ][ ui_vertex ]->coords;

			// check for double vertices
			if( cl_p != cl_pp )
			{
//				if( vcel[ ui_loop ][ ui_vertex ]->vertexinfo )
				{
					vpcl_new_points[ ui_new_num ] = mesh->AddVertex( cl_p );
//					std::cerr << ( void* ) vpcl_new_points[ ui_new_num ] << std::endl;
					if( vpcl_new_points[ ui_new_num ] != vcel[ ui_loop ][ ui_vertex ] )
					{
//						std::cerr << ( void* ) vpcl_new_points[ ui_new_num ] << " ";
//						std::cerr << vcel[ ui_loop ][ ui_vertex ]->edges.size( ) << " ";
//						std::cerr << "delete vertex2:" << ( void* ) vcel[ ui_loop ][ ui_vertex ] << std::endl;
//						std::cerr << vpcl_new_points[ ui_new_num ]->coords << " ";
//						delete vcel[ ui_loop ][ ui_vertex ];
//						std::cerr << vpcl_new_points[ ui_new_num ]->coords << std::endl;
					}
//					std::cerr << vpcl_new_points[ ui_new_num ]->coords << std::endl;
					++ui_new_num;
//					ui_last = ui_vertex;
				}
/*				else
				{
					const vec3d	ccl_pn = vcel[ ui_loop ][ ( ui_vertex + 1 ) % ui_size ]->coords;
					double		d_d1[ 2 ], d_d2[ 2 ], d_d3[ 2 ];

					d_d1[ 0 ] = ccl_pp.x;
					d_d1[ 1 ] = ccl_pp.y;
					d_d2[ 0 ] = ccl_p.x;
					d_d2[ 1 ] = ccl_p.y;
					d_d3[ 0 ] = ccl_pn.x;
					d_d3[ 1 ] = ccl_pn.y;

					if( orient2d( d_d1, d_d2, d_d3 ) != 0.0 )
					{
						vpcl_new_points[ ui_new_num ] = vcel[ ui_loop ][ ui_vertex ];
						++ui_new_num;
						ui_last = ui_vertex;
					}
				}*/
			}
			else
			{
				if( !mesh->findVertex( cl_p ) )
				{
//					std::cerr << "delete vertex3:" << ( void* ) vcel[ ui_loop ][ ui_vertex ] << std::endl;
					delete vcel[ ui_loop ][ ui_vertex ];
				}
			}
		}

		// copy new points
//		if( ui_new_num != ui_size )
		{
			ui_size = ui_new_num;
			vcel[ ui_loop ].resize( ui_size );
			for( ui_vertex = 0; ui_vertex < ui_size; ++ui_vertex )
			{
				vcel[ ui_loop ][ ui_vertex ] = vpcl_new_points[ ui_vertex ];
//				std::cerr << vpcl_new_points[ ui_vertex ]->coords << std::endl;
			}
		}

		// loops of size 2 are degenerate...
		if( ui_size > 2 )
		{
			ui_last = ui_size - 1;
			for( ui_vertex = 0; ui_vertex < ui_size; ++ui_vertex )
			{
				if( vcel[ ui_loop ][ ui_last ]->id != vcel[ ui_loop ][ ui_vertex ]->id )
				{
					// insert edge and events
//					std::cerr << "new edge " << vcel[ ui_loop ][ ui_last ]->id << " " << vcel[ ui_loop ][ ui_vertex ]->id << std::endl;
					pt_act_edge = new SScanLineEdge;
					pt_act_edge->pclFromVertex = vcel[ ui_loop ][ ui_last ];
					pt_act_edge->pclToVertex = vcel[ ui_loop ][ ui_vertex ];
//					std::cerr << pt_act_edge->pclFromVertex << " ";
//					std::cerr << pt_act_edge->pclToVertex << std::endl;
					pt_act_edge->ptPrev = NULL;
					pt_act_edge->ptNext = NULL;
					pt_act_edge->ptEntry = NULL;
//					pt_act_edge->uiOrigNum = vpt_edges.size( );
					pt_act_edge->bInvalid = false;
					if( insertScanLineEvents( pt_act_edge, spt_events ) )
					{
//						vpt_edges.push_back( pt_act_edge );
					}
					ui_last = ui_vertex;
				}
			}
		}
	}

	// we don't need the old edges any more...
	vcel.clear( );
//        std::cerr << "after clear: " << vcel.size() << std::endl;

	// construct new edge loops
	while( !spt_events.empty( ) )
	{
		ispt_act_event = spt_events.begin( );
		pt_act_event = *ispt_act_event;
//		std::cerr << "erase act event ";
		spt_events.erase( ispt_act_event );
//		std::cerr << "ok" << std::endl;
//		std::cerr << "pos is: " << pt_act_event->clPos << std::endl;
//		std::cerr << "other is: " << pt_act_event->clOther << std::endl;
//		std::cerr << "start is: " << pt_act_event->bStart << std::endl;

		// get actual edge
//		std::cerr << "event is: " << pt_act_event;
		pt_act_edge = pt_act_event->ptEdge;
//		std::cerr << " edge is: " << pt_act_edge << std::endl;
//		if( pt_act_event->bStart )
		if( pt_act_edge->ptEntry == NULL )
		{
			// create new scan line entry
			pt_act_entry = new SScanLineEntry;
			pt_act_entry->ptEdge = pt_act_edge;
			pt_act_edge->ptEntry = pt_act_entry;
//			std::cerr << pt_act_edge->ptEntry << " ";
			// sort into scan line
//			std::cerr << "scanline was: " << pt_scan_line << std::endl;
			pt_scan_line = insertInScanLine( pt_act_entry, pt_scan_line );
//			std::cerr << "scanline is: " << pt_scan_line << std::endl;
			// check if there is an intersection and split lines if there is one
			checkSLIntersection( pt_act_entry, spt_events );
//			std::cerr << pt_act_edge->ptEntry << std::endl;
		}
		else
		{
			// check if edge is valid
			i_inside = 0;
//			std::cerr << pt_act_edge->ptEntry << std::endl;
			if( !pt_act_edge->bInvalid )
			{
				pt_act_entry = pt_act_edge->ptEntry->ptNext;
				while( pt_act_entry )
				{
/*					std::cerr << pt_act_entry << std::endl;
					std::cerr << pt_act_entry->ptEdge->pclFromVertex->coords << " -> ";
					std::cerr << pt_act_entry->ptEdge->pclToVertex->coords << std::endl;*/
					if( pt_act_entry->ptEdge->pclFromVertex->coords > pt_act_entry->ptEdge->pclToVertex->coords )
					{
//						std::cerr << "+";
						// edge goes down => right is inside
						++i_inside;
					}
					else
					{
//						std::cerr << "-";
						// edge goes up => right is outside
						--i_inside;
					}
					pt_act_entry = pt_act_entry->ptNext;
				}
			}
			// remove it from the scanline
			pt_act_entry = pt_act_edge->ptEntry;
			pt_next_entry = pt_act_entry->ptNext;
			if( pt_act_entry->ptNext ) pt_act_entry->ptNext->ptPrev = pt_act_entry->ptPrev;
			if( pt_act_entry->ptPrev ) pt_act_entry->ptPrev->ptNext = pt_act_entry->ptNext;
			else pt_scan_line = pt_act_entry->ptNext;
			delete pt_act_entry;
			if( !pt_act_edge->bInvalid )
			{
				if( pt_act_edge->pclFromVertex->coords > pt_act_edge->pclToVertex->coords )
				{
					// edge goes down => must be closed
//					std::cerr << "inside = " << i_inside << ", should be -1" << std::endl;
					b_valid = ( i_inside == -1 );
				}
				else
				{
					// edge goes up => rest must be ok
//					std::cerr << "inside = " << i_inside << ", should be 0" << std::endl;
					b_valid = ( i_inside == 0 );
				}
				if( b_valid )
				{
					// append edge to polyline
					ui_size = vpt_start.size( );
					for( ui_loop = 0; ui_loop < ui_size; ++ui_loop )
					{
						if( pt_act_edge->pclToVertex == vpt_start[ ui_loop ]->pclFromVertex )
						{
//							std::cerr << "appending edge to polyline " << ui_loop << std::endl;
							pt_act_edge->ptNext = vpt_start[ ui_loop ];
							vpt_start[ ui_loop ]->ptPrev = pt_act_edge;
							vpt_start[ ui_loop ] = pt_act_edge;
							break;
						}
						if( pt_act_edge->pclFromVertex == vpt_end[ ui_loop ]->pclToVertex )
						{
//							std::cerr << "appending edge to polyline " << ui_loop << std::endl;
							pt_act_edge->ptPrev = vpt_end[ ui_loop ];
							vpt_end[ ui_loop ]->ptNext = pt_act_edge;
							vpt_end[ ui_loop ] = pt_act_edge;
							break;
						}
					}
					if( ui_loop < ui_size )
					{
						// check if polyline is closed
						if( vpt_start[ ui_loop ]->pclFromVertex == vpt_end[ ui_loop ]->pclToVertex )
						{
							// save closed polyline to vcel
//							std::cerr << "closing polyline " << ui_loop << std::endl;
							saveLoop( vpt_start[ ui_loop ] );
							if( ui_loop != ui_size - 1 )
							{
								vpt_start[ ui_loop ] = vpt_start[ ui_size - 1 ];
								vpt_end[ ui_loop ] = vpt_end[ ui_size - 1 ];
							}
							vpt_start.resize( ui_size - 1 );
							vpt_end.resize( ui_size - 1 );
						}
						else
						{
							// check if two polylines join
							for( ui_loop2 = 0; ui_loop2 < ui_size; ++ui_loop2 )
							{
								if( ui_loop != ui_loop2 )
								{
									if( vpt_start[ ui_loop ]->pclFromVertex == vpt_end[ ui_loop2 ]->pclToVertex )
									{
//										std::cerr << "joining polylines." << std::endl;
										vpt_start[ ui_loop ]->ptPrev = vpt_end[ ui_loop2 ];
										vpt_end[ ui_loop2 ]->ptNext = vpt_start[ ui_loop ];
										vpt_start[ ui_loop ] = vpt_start[ ui_loop2 ];
										break;
									}
									if( vpt_start[ ui_loop2 ]->pclFromVertex == vpt_end[ ui_loop ]->pclToVertex )
									{
//										std::cerr << "joining polylines." << std::endl;
										vpt_start[ ui_loop2 ]->ptPrev = vpt_end[ ui_loop ];
										vpt_end[ ui_loop ]->ptNext = vpt_start[ ui_loop2 ];
										vpt_end[ ui_loop ] = vpt_end[ ui_loop2 ];
										break;
									}
								}
							}
							if( ui_loop2 < ui_size )
							{
//								std::cerr << "removing old polyline " << ui_loop2 << std::endl;
								if( ui_loop2 != ui_size - 1 )
								{
									vpt_start[ ui_loop2 ] = vpt_start[ ui_size - 1 ];
									vpt_end[ ui_loop2 ] = vpt_end[ ui_size - 1 ];
								}
								vpt_start.resize( ui_size - 1 );
								vpt_end.resize( ui_size - 1 );
							}
						}
					}
					else
					{
//						std::cerr << "starting new polyline " << ui_loop << std::endl;
						// start new polyline
						vpt_start.push_back( pt_act_edge );
						vpt_end.push_back( pt_act_edge );
					}
				}
				else
				{
//					std::cerr << "delete edge " << pt_act_edge->pclFromVertex->id << " " << pt_act_edge->pclToVertex->id << std::endl;
					delete pt_act_edge;
				}
			}
			else
			{
//				std::cerr << "delete edge " << pt_act_edge->pclFromVertex->id << " " << pt_act_edge->pclToVertex->id << std::endl;
				delete pt_act_edge;
			}
			if( ( pt_next_entry ) && ( pt_next_entry->ptPrev ) )
			{
				if( ( !pt_next_entry->ptEdge->bInvalid ) &&
					( !pt_next_entry->ptPrev->ptEdge->bInvalid ) )
				{
//					std::cerr << pt_next_entry << ", " << pt_next_entry->ptPrev << std::endl;
					ScanLineIntersect( pt_next_entry->ptPrev, pt_next_entry, spt_events );
					if( pt_next_entry->ptEdge->bInvalid )
					{
						removeSLEntry( pt_next_entry->ptPrev, spt_events );
						removeSLEntry( pt_next_entry, spt_events );
					}
				}
			}
//			std::cerr << std::endl;
		}

		delete pt_act_event;
	}

/*	if( vpt_start.size( ) > 0 )
	{
		std::cerr.precision( 24 );
		std::cerr << vpt_start[ 0 ]->pclFromVertex->coords << " <-> " << vpt_end[ 0 ]->pclToVertex->coords << std::endl;
//		exit( -1 );
	}*/
	ui_size = vpt_start.size( );
	for( ui_loop = 0; ui_loop < ui_size; ++ui_loop )
	{
		// check if polyline is closed
//		if( vpt_start[ ui_loop ]->pclFromVertex->coords == vpt_end[ ui_loop ]->pclToVertex->coords )
		{
			// save closed polyline to vcel
//			std::cerr << "closing polyline " << ui_loop << std::endl;
			saveLoop( vpt_start[ ui_loop ] );
		}
	}
}

bool ParSpaceTrimmer::insertScanLineEvents( SScanLineEdge *ptEdge, ScanLineEventSet &rsptEvents, char cWhich )
{
	SScanLineEvent								*pt_new_event;
	std::pair< ScanLineEventSet::iterator, bool >	pr_insert;
	vec2d										min;
	vec2d										max;
/*	int											i_select = 0;

	if( ptEdge->pclFromVertex->coords.y < ptEdge->pclToVertex->coords.y ) i_select = 1;
	else if( ptEdge->pclFromVertex->coords.y > ptEdge->pclToVertex->coords.y ) i_select = -1;
	else if( ptEdge->pclFromVertex->coords.x < ptEdge->pclToVertex->coords.x ) i_select = 1;
	else if( ptEdge->pclFromVertex->coords.x > ptEdge->pclToVertex->coords.x ) i_select = -1;*/

	if( ptEdge->pclFromVertex->coords < ptEdge->pclToVertex->coords )
//	if( i_select == 1 )
	{
		min.x = ptEdge->pclFromVertex->coords.x;
		min.y = ptEdge->pclFromVertex->coords.y;
		max.x = ptEdge->pclToVertex->coords.x;
		max.y = ptEdge->pclToVertex->coords.y;
	}
	else if( ptEdge->pclFromVertex->coords == ptEdge->pclToVertex->coords )
//	else if( i_select == 0 )
	{
		return false;
	}
	else
	{
		min.x = ptEdge->pclToVertex->coords.x;
		min.y = ptEdge->pclToVertex->coords.y;
		max.x = ptEdge->pclFromVertex->coords.x;
		max.y = ptEdge->pclFromVertex->coords.y;
	}

	if( cWhich != 1 )
	{
		pt_new_event = new SScanLineEvent;
		pt_new_event->ptEdge = ptEdge;
		pt_new_event->bStart = true;
		pt_new_event->clPos = min;
		pt_new_event->clOther = max;

/*		std::cerr << "scan line event: edge = " << pt_new_event->ptEdge;
		std::cerr << " start = " << pt_new_event->bStart;
		std::cerr << " pos = " << pt_new_event->clPos;
		std::cerr << " other = " << pt_new_event->clOther << std::endl;*/
//		std::cerr << " num = " << pt_new_event->ptEdge->uiOrigNum << std::endl;

		pr_insert = rsptEvents.insert( pt_new_event );
		if( !pr_insert.second )
		{
			std::cerr << "Scan line event exists! aborting." << std::endl;
			exit( -1 );
		}
	}

	if( cWhich != 0 )
	{
		pt_new_event = new SScanLineEvent;
		pt_new_event->ptEdge = ptEdge;
		pt_new_event->bStart = false;
		pt_new_event->clPos = max;
		pt_new_event->clOther = min;

/*		std::cerr << "scan line event: edge = " << pt_new_event->ptEdge;
		std::cerr << " start = " << pt_new_event->bStart;
		std::cerr << " pos = " << pt_new_event->clPos;
		std::cerr << " other = " << pt_new_event->clOther << std::endl;*/
//		std::cerr << " num = " << pt_new_event->ptEdge->uiOrigNum << std::endl;

		pr_insert = rsptEvents.insert( pt_new_event );
		if( !pr_insert.second )
		{
			std::cerr << "Scan line event exists! aborting." << std::endl;
			exit( -1 );
		}
	}

	return true;
}

SScanLineEntry *ParSpaceTrimmer::insertInScanLine( SScanLineEntry *ptActEntry, SScanLineEntry *ptScanLine )
{
	SScanLineEntry	*pt_prev_entry = NULL;
	SScanLineEntry	*pt_next_entry;

	pt_next_entry = ptScanLine;
	while( pt_next_entry )
	{
		if( isSLEntryLess( ptActEntry->ptEdge, pt_next_entry->ptEdge ) )
			break;
		pt_prev_entry = pt_next_entry;
		pt_next_entry = pt_next_entry->ptNext;
	}
	ptActEntry->ptPrev = pt_prev_entry;
	ptActEntry->ptNext = pt_next_entry;
	if( pt_next_entry ) pt_next_entry->ptPrev = ptActEntry;
	if( pt_prev_entry ) pt_prev_entry->ptNext = ptActEntry;
	else return ptActEntry;
	return ptScanLine;
}

void ParSpaceTrimmer::saveLoop( SScanLineEdge *ptEdge )
{
	SScanLineEdge			*pt_del;
	std::vector< DCTPVertex* >	vpcl_cel;

	if( ptEdge == NULL )
	{
		return;
	}

	while( ptEdge )
	{
		vpcl_cel.push_back( ptEdge->pclFromVertex );
		pt_del = ptEdge;
		ptEdge = ptEdge->ptNext;
//		std::cerr << "delete edge " << pt_del->pclFromVertex->id << " " << pt_del->pclToVertex->id << std::endl;
		delete pt_del;
	}

	if( vpcl_cel.size( ) == 1 )
	{
		return;
	}

	// closed polylines have the same first and last vertex
	vpcl_cel.push_back( vpcl_cel[ 0 ] );

	vcel.push_back( vpcl_cel );
}

void ParSpaceTrimmer::checkSLIntersection( SScanLineEntry *ptActEntry, ScanLineEventSet &rsptEvents )
{
	SScanLineEntry	*pt_prev_entry = ptActEntry->ptPrev;
	SScanLineEntry	*pt_next_entry = ptActEntry->ptNext;
//	DCTPVertex		*pcl_split_vertex;

	if( pt_prev_entry )
	{
		ScanLineIntersect( ptActEntry, pt_prev_entry, rsptEvents );
		if( ptActEntry->ptEdge->bInvalid )
		{
			removeSLEntry( ptActEntry->ptPrev, rsptEvents );
			removeSLEntry( ptActEntry, rsptEvents );
			return;
		}
	}
	if( pt_next_entry )
	{
		ScanLineIntersect( ptActEntry, pt_next_entry, rsptEvents );
		if( ptActEntry->ptEdge->bInvalid )
		{
			removeSLEntry( ptActEntry, rsptEvents );
			removeSLEntry( ptActEntry->ptNext, rsptEvents );
		}
	}
}

void ParSpaceTrimmer::ScanLineIntersect( SScanLineEntry *ptEntry1, SScanLineEntry *ptEntry2, ScanLineEventSet &rsptEvents )
{
	SScanLineEdge	*pt_edge1 = ptEntry1->ptEdge;
	SScanLineEdge	*pt_edge2 = ptEntry2->ptEdge;
	SScanLineEdge	*pt_new_edge;
	DCTPVertex		*pcl_split_vertex;

/*	if( ( pt_edge1->pclFromVertex == pt_edge2->pclFromVertex ) ||
		( pt_edge1->pclFromVertex == pt_edge2->pclToVertex ) ||
		( pt_edge1->pclToVertex == pt_edge2->pclFromVertex ) ||
		( pt_edge1->pclToVertex == pt_edge2->pclToVertex ) )
	{
		return;
	}*/

/*	if( ( pt_edge1->bInvalid ) || ( pt_edge2->bInvalid ) )
	{
		std::cerr << pt_edge1 << " " << pt_edge2 << std::endl;
		exit( -1 );
	}*/

//	std::cerr.precision( 12 );

	double ad_s1[ 2 ];
	double ad_e1[ 2 ];
	double ad_s2[ 2 ];
	double ad_e2[ 2 ];

	ad_s1[ 0 ] = pt_edge1->pclFromVertex->coords.x;
	ad_s1[ 1 ] = pt_edge1->pclFromVertex->coords.y;
	ad_e1[ 0 ] = pt_edge1->pclToVertex->coords.x;
	ad_e1[ 1 ] = pt_edge1->pclToVertex->coords.y;
	ad_s2[ 0 ] = pt_edge2->pclFromVertex->coords.x;
	ad_s2[ 1 ] = pt_edge2->pclFromVertex->coords.y;
	ad_e2[ 0 ] = pt_edge2->pclToVertex->coords.x;
	ad_e2[ 1 ] = pt_edge2->pclToVertex->coords.y;

	const double cd_r1 = orient2d( ad_s1, ad_s2, ad_e2 );
	const double cd_r2 = orient2d( ad_e1, ad_s2, ad_e2 );
	if( ( ( cd_r1 < 0.0 ) && ( cd_r2 < 0.0 ) ) ||
		( ( cd_r1 > 0.0 ) && ( cd_r2 > 0.0 ) ) )
	{
		return;
	}

	const double cd_r3 = orient2d( ad_s2, ad_s1, ad_e1 );
	const double cd_r4 = orient2d( ad_e2, ad_s1, ad_e1 );
        
                double  cd_v1x = ad_e1[ 0 ] - ad_s1[ 0 ];
		double	cd_v1y = ad_e1[ 1 ] - ad_s1[ 1 ];
		double	cd_v2x = ad_e2[ 0 ] - ad_s2[ 0 ];
		double	cd_v2y = ad_e2[ 1 ] - ad_s2[ 1 ];

		double	cd_b = cd_v1x * cd_v2y - cd_v1y * cd_v2x;

    	if( ( ( cd_r3 < 0.0 ) && ( cd_r4 < 0.0 ) ) ||
		( ( cd_r3 > 0.0 ) && ( cd_r4 > 0.0 ) ) )
	{
		return;
	}

	if( (( cd_r1 == 0.0 ) && ( cd_r2 == 0.0 ) &&
	     ( cd_r3 == 0.0 ) && ( cd_r4 == 0.0 ) ) ||
                fabs(cd_b) < 1e-32 //prevent strange roundoff error later 
                                   //(this shouldn't be needed in an ideal world...)
                )
	{
		// edges are coliniear
		DCTPVertex	*pcl_ip1;
		DCTPVertex	*pcl_ip2;
		bool		b_down1;
		bool		b_down2;

//                std::cerr <<"collinear edges..."<<std::endl;

/*                if ( fabs(cd_b) < 1e-32 && 
                     ( cd_r1 != 0.0 || cd_r1 != 0.0 || 
                       cd_r3 != 0.0 || cd_r4 != 0.0 ) ) 
                    std::cerr <<"collinear because of cd_b" << cd_b <<std::endl;*/
                
		b_down1 = ( pt_edge1->pclFromVertex->coords < pt_edge1->pclToVertex->coords );
		b_down2 = ( pt_edge2->pclFromVertex->coords < pt_edge2->pclToVertex->coords );
		pcl_ip1 = b_down1 ? pt_edge1->pclFromVertex : pt_edge1->pclToVertex;
		pcl_ip2 = b_down2 ? pt_edge2->pclFromVertex : pt_edge2->pclToVertex;

		if( pcl_ip1->coords == pcl_ip2->coords )
		{
			// edges are colinear and start in the same point.
			// => mark them as invalid if they have opposite directions
			//    and use the end points
			if( ( ( b_down1 ) && !( b_down2 ) ) || ( !( b_down1 ) && ( b_down2 ) ) )
			{
//				std::cerr << "edges become invalid." << std::endl;
				pt_edge1->bInvalid = true;
				pt_edge2->bInvalid = true;
			}
			pcl_ip1 = b_down1 ? pt_edge1->pclToVertex : pt_edge1->pclFromVertex;
			pcl_ip2 = b_down2 ? pt_edge2->pclToVertex : pt_edge2->pclFromVertex;

			pcl_split_vertex = ( pcl_ip1->coords < pcl_ip2->coords ) ?
							   pcl_ip1 : pcl_ip2;
		}
		else
		{
			pcl_split_vertex = ( pcl_ip1->coords > pcl_ip2->coords ) ?
							   pcl_ip1 : pcl_ip2;
		}
	}
	else
	{
		// we have an intersection => find point
//		const double	cd_v1x = ad_e1[ 0 ] - ad_s1[ 0 ];
//		const double	cd_v1y = ad_e1[ 1 ] - ad_s1[ 1 ];
//		const double	cd_v2x = ad_e2[ 0 ] - ad_s2[ 0 ];
//		const double	cd_v2y = ad_e2[ 1 ] - ad_s2[ 1 ];

		if( ( cd_v1x * cd_v1x + cd_v1y * cd_v1y ) < ( cd_v2x * cd_v2x + cd_v2y * cd_v2y ) )
		{
			const double	cd_ppx = ad_s2[ 0 ] - ad_s1[ 0 ];
			const double	cd_ppy = ad_s2[ 1 ] - ad_s1[ 1 ];
			const double	cd_a = cd_ppx * cd_v2y - cd_ppy * cd_v2x;
//			const double	cd_b = cd_v1x * cd_v2y - cd_v1y * cd_v2x;
			double			d_q = cd_a / cd_b;
//			std::cerr << cd_a << " / " << cd_b << " = " << d_q;
//			std::cerr << cd_r1 << " " << cd_r2 << " " << cd_r3 << " " << cd_r4 << std::endl;

			if( d_q < DCTP_EPS )
			{
				pcl_split_vertex = pt_edge1->pclFromVertex;
			}
			else if( 1.0 - d_q < DCTP_EPS )
			{
				pcl_split_vertex = pt_edge1->pclToVertex;
			}
			else
			{
				vec3d	cl_ip;

				cl_ip.x = pt_edge1->pclFromVertex->coords.x + cd_v1x * d_q;
				cl_ip.y = pt_edge1->pclFromVertex->coords.y + cd_v1y * d_q;
//				cl_ip.x = 1e-7 * floor( ( pt_edge1->pclFromVertex->coords.x + cd_v1x * d_q ) / 1e-7 );
//				cl_ip.y = 1e-7 * floor( ( pt_edge1->pclFromVertex->coords.y + cd_v1y * d_q ) / 1e-7 );
				cl_ip.z = 0.0;
				pcl_split_vertex = mesh->AddVertex( cl_ip );
			}
		}
		else
		{
			const double	cd_ppx = ad_s1[ 0 ] - ad_s2[ 0 ];
			const double	cd_ppy = ad_s1[ 1 ] - ad_s2[ 1 ];
			const double	cd_a = cd_ppx * cd_v1y - cd_ppy * cd_v1x;
			const double	cd_b = cd_v2x * cd_v1y - cd_v2y * cd_v1x;
			double			d_q = cd_a / cd_b;
//			std::cerr << cd_a << " / " << cd_b << " = " << d_q;
//			std::cerr << cd_r1 << " " << cd_r2 << " " << cd_r3 << " " << cd_r4 << std::endl;

			if( d_q < DCTP_EPS )
			{
				pcl_split_vertex = pt_edge2->pclFromVertex;
			}
			else if( 1.0 - d_q < DCTP_EPS )
			{
				pcl_split_vertex = pt_edge2->pclToVertex;
			}
			else
			{
				vec3d	cl_ip;

				cl_ip.x = pt_edge2->pclFromVertex->coords.x + cd_v2x * d_q;
				cl_ip.y = pt_edge2->pclFromVertex->coords.y + cd_v2y * d_q;
//				cl_ip.x = 1e-7 * floor( ( pt_edge2->pclFromVertex->coords.x + cd_v2x * d_q ) / 1e-7 );
//				cl_ip.y = 1e-7 * floor( ( pt_edge2->pclFromVertex->coords.y + cd_v2y * d_q ) / 1e-7 );
				cl_ip.z = 0.0;
				pcl_split_vertex = mesh->AddVertex( cl_ip );
			}
		}
	}

//	std::cerr << "intersection at " << pcl_split_vertex->coords << std::endl;
//	std::cerr << "edge1: " << pt_edge1->pclFromVertex->coords << " - " << pt_edge1->pclToVertex->coords << std::endl;
//	std::cerr << "edge2: " << pt_edge2->pclFromVertex->coords << " - " << pt_edge2->pclToVertex->coords << std::endl;

	// now we have the point => split edges
	if( ( pcl_split_vertex != pt_edge1->pclFromVertex ) &&
		( pcl_split_vertex != pt_edge1->pclToVertex ) )
	{
		// split edge1
//		std::cerr << "spliting edge1: " << pt_edge1->pclFromVertex->coords << " - " << pt_edge1->pclToVertex->coords << std::endl;
		pt_new_edge = new SScanLineEdge;
		if( pt_edge1->pclFromVertex->coords < pt_edge1->pclToVertex->coords )
		{
			pt_new_edge->pclFromVertex = pt_edge1->pclFromVertex;
			if( pt_edge1->bInvalid )
				pt_new_edge->pclToVertex = pt_edge1->pclToVertex;
			else
				pt_new_edge->pclToVertex = pcl_split_vertex;
			pt_edge1->pclFromVertex = pcl_split_vertex;
		}
		else
		{
			pt_new_edge->pclToVertex = pt_edge1->pclToVertex;
			if( pt_edge1->bInvalid )
				pt_new_edge->pclFromVertex = pt_edge1->pclFromVertex;
			else
				pt_new_edge->pclFromVertex = pcl_split_vertex;
			pt_edge1->pclToVertex = pcl_split_vertex;
		}
		pt_new_edge->ptPrev = pt_edge1->ptPrev;
		if( pt_new_edge->ptPrev ) pt_new_edge->ptPrev->ptNext = pt_new_edge;
		pt_new_edge->ptNext = pt_edge1->ptNext;
		if( pt_new_edge->ptNext ) pt_new_edge->ptNext->ptPrev = pt_new_edge;
		pt_new_edge->ptEntry = pt_edge1->ptEntry;
		pt_new_edge->ptEntry->ptEdge = pt_new_edge;
//		pt_new_edge->uiOrigNum = pt_edge1->uiOrigNum;
		pt_new_edge->bInvalid = pt_edge1->bInvalid;
		pt_edge1->ptPrev = NULL;
		pt_edge1->ptNext = NULL;
		pt_edge1->ptEntry = NULL;
		pt_edge1->bInvalid = false;
		insertScanLineEvents( pt_new_edge, rsptEvents, 1 ); // first event was this
		insertScanLineEvents( pt_edge1, rsptEvents, 0 );    // second event already exists
//		std::cerr << "new edge1: " << pt_edge1->pclFromVertex->coords << " - " << pt_edge1->pclToVertex->coords << std::endl;
//		std::cerr << "new edge: " << pt_new_edge->pclFromVertex->coords << " - " << pt_new_edge->pclToVertex->coords << std::endl;
		pcl_split_vertex->vertexinfo = ( void* ) 1;
	}
	if( ( pcl_split_vertex != pt_edge2->pclFromVertex ) &&
		( pcl_split_vertex != pt_edge2->pclToVertex ) )
	{
		// split edge2
//		std::cerr << "spliting edge2: " << pt_edge2->pclFromVertex->coords << " - " << pt_edge2->pclToVertex->coords << std::endl;
		pt_new_edge = new SScanLineEdge;
		if( pt_edge2->pclFromVertex->coords < pt_edge2->pclToVertex->coords )
		{
			pt_new_edge->pclFromVertex = pt_edge2->pclFromVertex;
			if( pt_edge2->bInvalid )
				pt_new_edge->pclToVertex = pt_edge2->pclToVertex;
			else
				pt_new_edge->pclToVertex = pcl_split_vertex;
			pt_edge2->pclFromVertex = pcl_split_vertex;
		}
		else
		{
			pt_new_edge->pclToVertex = pt_edge2->pclToVertex;
			if( pt_edge2->bInvalid )
				pt_new_edge->pclFromVertex = pt_edge2->pclFromVertex;
			else
				pt_new_edge->pclFromVertex = pcl_split_vertex;
			pt_edge2->pclToVertex = pcl_split_vertex;
		}
		pt_new_edge->ptPrev = pt_edge2->ptPrev;
		if( pt_new_edge->ptPrev ) pt_new_edge->ptPrev->ptNext = pt_new_edge;
		pt_new_edge->ptNext = pt_edge2->ptNext;
		if( pt_new_edge->ptNext ) pt_new_edge->ptNext->ptPrev = pt_new_edge;
		pt_new_edge->ptEntry = pt_edge2->ptEntry;
		pt_new_edge->ptEntry->ptEdge = pt_new_edge;
//		pt_new_edge->uiOrigNum = pt_edge2->uiOrigNum;
		pt_new_edge->bInvalid = pt_edge2->bInvalid;
		pt_edge2->ptPrev = NULL;
		pt_edge2->ptNext = NULL;
		pt_edge2->ptEntry = NULL;
		pt_edge2->bInvalid = false;
		insertScanLineEvents( pt_new_edge, rsptEvents, 1 ); // first event was this
		insertScanLineEvents( pt_edge2, rsptEvents, 0 );    // second event already exists
//		std::cerr << "new edge2: " << pt_edge2->pclFromVertex->coords << " - " << pt_edge2->pclToVertex->coords << std::endl;
//		std::cerr << "new edge: " << pt_new_edge->pclFromVertex->coords << " - " << pt_new_edge->pclToVertex->coords << std::endl;
		pcl_split_vertex->vertexinfo = ( void* ) 1;
	}
}

bool ParSpaceTrimmer::isSLEntryLess( SScanLineEdge *ptEdge1, SScanLineEdge *ptEdge2 )
{
	vec2d	top1;
	vec2d	bottom1;
	vec2d	top2;
	vec2d	bottom2;

/*	std::cerr << ptEdge1 << " " << ptEdge2 << std::endl;
	std::cerr << ptEdge1->pclFromVertex << " " << ptEdge1->pclToVertex << std::endl;
	std::cerr << ptEdge2->pclFromVertex << " " << ptEdge2->pclToVertex << std::endl;*/

	if( ptEdge1->pclFromVertex->coords < ptEdge1->pclToVertex->coords )
	{
		top1.x = ptEdge1->pclFromVertex->coords.x;
		top1.y = ptEdge1->pclFromVertex->coords.y;
		bottom1.x = ptEdge1->pclToVertex->coords.x;
		bottom1.y = ptEdge1->pclToVertex->coords.y;
	}
	else
	{
		top1.x = ptEdge1->pclToVertex->coords.x;
		top1.y = ptEdge1->pclToVertex->coords.y;
		bottom1.x = ptEdge1->pclFromVertex->coords.x;
		bottom1.y = ptEdge1->pclFromVertex->coords.y;
	}
	if( ptEdge2->pclFromVertex->coords < ptEdge2->pclToVertex->coords )
	{
		top2.x = ptEdge2->pclFromVertex->coords.x;
		top2.y = ptEdge2->pclFromVertex->coords.y;
		bottom2.x = ptEdge2->pclToVertex->coords.x;
		bottom2.y = ptEdge2->pclToVertex->coords.y;
	}
	else
	{
		top2.x = ptEdge2->pclToVertex->coords.x;
		top2.y = ptEdge2->pclToVertex->coords.y;
		bottom2.x = ptEdge2->pclFromVertex->coords.x;
		bottom2.y = ptEdge2->pclFromVertex->coords.y;
	}
	if( top1 != top2 )
	{
		double ad_s1[ 2 ];
//		double ad_e1[ 2 ];
		double ad_s2[ 2 ];
		double ad_e2[ 2 ];

		ad_s1[ 0 ] = top1.x;
		ad_s1[ 1 ] = top1.y;
//		ad_e1[ 0 ] = bottom1.x;
//		ad_e1[ 1 ] = bottom1.y;
		ad_s2[ 0 ] = top2.x;
		ad_s2[ 1 ] = top2.y;
		ad_e2[ 0 ] = bottom2.x;
		ad_e2[ 1 ] = bottom2.y;

		const double cd_orient = orient2d( ad_s2, ad_e2, ad_s1 );
		if( cd_orient != 0.0 )
		{
			return ( cd_orient > 0.0 );
		}
	}

	// check directions
	vec2d		dir1 = bottom1 - top1;
	vec2d		dir2 = bottom2 - top2;

	if( fabs( dir1.y ) < DCTP_EPS )
	{
		if( fabs( dir2.y ) < DCTP_EPS )
		{
			return ( dir1.x < dir2.x );
		}
		return ( dir1.x < 0.0 ); // this can't be zero!
	}
	if( fabs( dir2.y ) < DCTP_EPS )
	{
		return ( dir2.x > 0.0 ); // this can't be zero!
	}

	const double	r1 = dir1.x / dir1.y;
	const double	r2 = dir2.x / dir2.y;

	if( fabs( r1 - r2 ) >= DCTP_EPS )
	{
		return ( r1 < r2 );
	}

	// this can only be false for both ways, if it is the same edge!
	return( ( ( int ) ptEdge1 ) < ( ( int ) ptEdge2 ) );
}

void ParSpaceTrimmer::removeSLEntry( SScanLineEntry *ptEntry, ScanLineEventSet &rsptEvents )
{
	SScanLineEvent								*pt_new_event;
	std::pair< ScanLineEventSet::iterator, bool >	pr_insert;
	vec2d										min;
	vec2d										max;

	if( ptEntry->ptEdge->pclFromVertex->coords < ptEntry->ptEdge->pclToVertex->coords )
	{
		min.x = ptEntry->ptEdge->pclFromVertex->coords.x;
		min.y = ptEntry->ptEdge->pclFromVertex->coords.y;
		max.x = ptEntry->ptEdge->pclToVertex->coords.x;
		max.y = ptEntry->ptEdge->pclToVertex->coords.y;
	}
	else if( ptEntry->ptEdge->pclFromVertex->coords == ptEntry->ptEdge->pclToVertex->coords )
	{
		return;
	}
	else
	{
		min.x = ptEntry->ptEdge->pclToVertex->coords.x;
		min.y = ptEntry->ptEdge->pclToVertex->coords.y;
		max.x = ptEntry->ptEdge->pclFromVertex->coords.x;
		max.y = ptEntry->ptEdge->pclFromVertex->coords.y;
	}

	// remove old event
	pt_new_event = new SScanLineEvent;
	pt_new_event->ptEdge = ptEntry->ptEdge;
	pt_new_event->bStart = false;
	pt_new_event->clPos = max;
	pt_new_event->clOther = min;

/*	std::cerr << "remove scan line event: edge = " << pt_new_event->ptEdge;
	std::cerr << " start = " << pt_new_event->bStart;
	std::cerr << " pos = " << pt_new_event->clPos;
	std::cerr << " other = " << pt_new_event->clOther << std::endl;*/
//	std::cerr << " num = " << pt_new_event->ptEdge->uiOrigNum << std::endl;

	unsigned int	ui_old_size = rsptEvents.size( );
//	std::cerr << "remove scan line entry ";
	rsptEvents.erase( pt_new_event );
//	std::cerr << "ok";
	delete pt_new_event;

	if( rsptEvents.size( ) == ui_old_size )
	{
//		std::cerr << "Event not found! searching: " << ptEntry->ptEdge << std::endl;
		for( ScanLineEventSet::iterator i = rsptEvents.begin( ); i != rsptEvents.end( ); ++i )
		{
/*			if( (*i)->bStart == 0 )
				std::cerr << (*i)->ptEdge << std::endl;*/
			if( /*( ( *i )->bStart == 0 ) &&*/ ( ( *i )->ptEdge == ptEntry->ptEdge ) )
			{
				rsptEvents.erase( i );
				break;
			}
		}
//		exit( -1 );
//		return;
	}

	// insert new event to delete edge instantly
	pt_new_event = new SScanLineEvent;
	pt_new_event->ptEdge = ptEntry->ptEdge;
	pt_new_event->bStart = false;
	pt_new_event->clPos = min;
	pt_new_event->clPos.y -= 1.0;
	pt_new_event->clOther = min;

/*	std::cerr << "replace scan line event: edge = " << pt_new_event->ptEdge;
	std::cerr << " start = " << pt_new_event->bStart;
	std::cerr << " pos = " << pt_new_event->clPos;
	std::cerr << " other = " << pt_new_event->clOther << std::endl;*/
//	std::cerr << " num = " << pt_new_event->ptEdge->uiOrigNum << std::endl;
	pr_insert = rsptEvents.insert( pt_new_event );
	if( !pr_insert.second )
	{
		std::cerr << "Scan line event exists! aborting." << std::endl;
		exit( -1 );
	}

}

DCTPVertex *ParSpaceTrimmer::intersectsLoop( DCTPVertex *pclVertex1, DCTPVertex *pclVertex2, unsigned int uiLoop )
{
	unsigned int	ui_vertex;
	unsigned int	ui_size = vcel[ uiLoop ].size( );
	DCTPVertex		*pcl_prev_vertex;
	DCTPVertex		*pcl_act_vertex = vcel[ uiLoop ][ ui_size - 1 ];
	vec3d			cl_old;

	for( ui_vertex = 0; ui_vertex < ui_size; ++ui_vertex )
	{
		pcl_prev_vertex = pcl_act_vertex;
		pcl_act_vertex = vcel[ uiLoop ][ ui_vertex ];

		if( ( pclVertex1->coords != pcl_prev_vertex->coords ) &&
			( pclVertex1->coords != pcl_act_vertex->coords ) &&
			( pclVertex2->coords != pcl_prev_vertex->coords ) &&
			( pclVertex2->coords != pcl_act_vertex->coords ) )
		{

			double ad_s1[ 2 ];
			double ad_e1[ 2 ];
			double ad_s2[ 2 ];
			double ad_e2[ 2 ];

			ad_s1[ 0 ] = pclVertex1->coords.x;
			ad_s1[ 1 ] = pclVertex1->coords.y;
			ad_e1[ 0 ] = pclVertex2->coords.x;
			ad_e1[ 1 ] = pclVertex2->coords.y;
			ad_s2[ 0 ] = pcl_prev_vertex->coords.x;
			ad_s2[ 1 ] = pcl_prev_vertex->coords.y;
			ad_e2[ 0 ] = pcl_act_vertex->coords.x;
			ad_e2[ 1 ] = pcl_act_vertex->coords.y;

			const double cd_r1 = orient2d( ad_s1, ad_e1, ad_s2 );
			const double cd_r2 = orient2d( ad_s1, ad_e1, ad_e2 );
			if( ( ( cd_r1 >= 0.0 ) || ( cd_r2 >= 0.0 ) ) &&
				( ( cd_r1 <= 0.0 ) || ( cd_r2 <= 0.0 ) ) )
			{

				const double cd_r3 = orient2d( ad_s2, ad_e2, ad_s1 );
				const double cd_r4 = orient2d( ad_s2, ad_e2, ad_e1 );
				if( ( ( cd_r3 >= 0.0 ) || ( cd_r4 >= 0.0 ) ) &&
					( ( cd_r3 <= 0.0 ) || ( cd_r4 <= 0.0 ) ) )
				{
					if( ( cd_r1 == 0.0 ) && ( cd_r2 == 0.0 ) && ( cd_r3 == 0.0 ) && ( cd_r4 == 0.0 ) )
					{
						const vec3d		ccl_v12 = pclVertex2->coords - pclVertex1->coords;
						const double	cd_prod_a =  ccl_v12.dot( pcl_act_vertex->coords - pclVertex1->coords );
						const double	cd_prod_p =  ccl_v12.dot( pcl_prev_vertex->coords - pclVertex1->coords );
						const double	cd_qsize = ccl_v12.quad_size( );

						if( ( cd_prod_a >= 0.0 ) && ( cd_prod_a <= cd_qsize ) )
						{
							if( ( cd_prod_p >= 0.0 ) && ( cd_prod_p <= cd_prod_a ) )
							{
								return pcl_prev_vertex;
							}
							else
							{
								return pcl_act_vertex;
							}
						}
						else if( ( cd_prod_p >= 0.0 ) && ( cd_prod_p <= cd_qsize ) )
						{
							return pcl_prev_vertex;
						}
					}
					else
					{
//						std::cerr << pclVertex1->coords << "->" << pclVertex2->coords << std::endl;
//						std::cerr << pcl_prev_vertex->coords << "->" << pcl_act_vertex->coords << std::endl;
						cl_old = pclVertex1->coords - pclVertex2->coords;
						if( ( vec2d( pclVertex1->coords - pcl_prev_vertex->coords ) ).dot( cl_old ) -
							( vec2d( pclVertex1->coords - pcl_act_vertex->coords ) ).dot( cl_old ) < DCTP_EPS )
						{
//							std::cerr << "=>" << pcl_prev_vertex->coords << std::endl;
							return pcl_prev_vertex;
						}
//						std::cerr << "=>" << pcl_act_vertex->coords << std::endl;
						return pcl_act_vertex;
					}
				}
			}
		}
	}

	return false;
}

bool ParSpaceTrimmer::doIntersect( vec2d clV1, vec2d clV2, vec2d clV3, vec2d clV4, double &rdParam )
{
	if( ( clV1 == clV3 ) || ( clV1 == clV4 ) )
	{
		if( clV1 == clV3 )
		{
			clV3 = clV4;
		}
        const vec2d v = clV3 - clV1;
        const vec2d w = clV2 - clV1;
        const double c1 = v.dot( w );
        const double c2 = w.quad_size( );

//		std::cerr.precision( 10 );
//		std::cerr << clV1 << "->" << clV2 << "," << clV3;

//		std::cerr << ":" << c2 << "," << c1 << std::endl;

		if( c1 <= 0.0 )
		{
			return false;	// other direction
		}
		else if( c1 < c2 )
		{
			if( clV1 + ( w * ( c1 / c2 ) ) != clV3 )
				return false;	// too far away
		}
		else
		{
			if( clV2 != ( clV1 + v * ( c2 / c1 ) ) )
				return false;	// too far away
		}
		rdParam = c1 / c2;
		if( rdParam < DCTP_EPS )
		{
			return false;
		}
		return true;
	}

	double ad_s1[ 2 ];
	double ad_e1[ 2 ];
	double ad_s2[ 2 ];
	double ad_e2[ 2 ];

	ad_s1[ 0 ] = clV1.x;
	ad_s1[ 1 ] = clV1.y;
	ad_e1[ 0 ] = clV2.x;
	ad_e1[ 1 ] = clV2.y;
	ad_s2[ 0 ] = clV3.x;
	ad_s2[ 1 ] = clV3.y;
	ad_e2[ 0 ] = clV4.x;
	ad_e2[ 1 ] = clV4.y;

	const double cd_r3 = orient2d( ad_s2, ad_s1, ad_e1 );
	const double cd_r4 = orient2d( ad_e2, ad_s1, ad_e1 );
	if( ( ( cd_r3 < 0.0 ) && ( cd_r4 < 0.0 ) ) ||
		( ( cd_r3 > 0.0 ) && ( cd_r4 > 0.0 ) ) )
	{
		return false;
	} 

	const double cd_r1 = orient2d( ad_s1, ad_s2, ad_e2 );
	const double cd_r2 = orient2d( ad_e1, ad_s2, ad_e2 );
	if( ( ( cd_r1 < 0.0 ) && ( cd_r2 < 0.0 ) ) ||
		( ( cd_r1 > 0.0 ) && ( cd_r2 > 0.0 ) ) )
	{
        const vec2d v = clV4 - clV3;
        const vec2d w = clV2 - clV3;
        const double c1 = v.dot( w );
        const double c2 = v.quad_size( );
		if( c1 <= 0.0 )
		{
			if( clV2 != clV3 )
				return false;
		}
		else if( c1 > c2 )
		{
			if( clV2 != clV4 )
				return false;
		}
		else
		{
			if( clV2 != ( clV3 + v * ( c1 / c2 ) ) )
				return false;
//			std::cerr << clV2 << "<->" << ( clV3 + v * ( c1 / c2 ) ) << " (" << ( c1 / c2 ) << ")" << std::endl;
		}
//		std::cerr << "end near edge" << std::endl;
		rdParam = 1.0;
		return true;
	}

//	std::cerr << clV1 << "->" << clV2 << std::endl;
//	std::cerr << clV3 << "->" << clV4 << std::endl;
//	std::cerr << cd_r1 << " " << cd_r2 << " " << cd_r3 << " " << cd_r4 << std::endl;

	if( ( cd_r1 == 0.0 ) && ( cd_r2 == 0.0 ) &&
		( cd_r3 == 0.0 ) && ( cd_r4 == 0.0 ) )
	{
//		std::cerr.precision( 12 );
		// edges are coliniear
		double	d_tmp;

		rdParam = -1.0;
		d_tmp = ( ( clV4.x - clV1.x ) * ( clV2.x - clV1.x )
				+ ( clV4.y - clV1.y ) * ( clV2.y - clV1.y ) )
			  / ( clV2 - clV1 ).quad_size( );
//		std::cerr << d_tmp << " ";
		if( d_tmp > DCTP_EPS ) rdParam = d_tmp;
		d_tmp = ( ( clV3.x - clV1.x ) * ( clV2.x - clV1.x )
				+ ( clV3.y - clV1.y ) * ( clV2.y - clV1.y ) )
			  / ( clV2 - clV1 ).quad_size( );
//		std::cerr << d_tmp << std::endl;
		if( d_tmp > DCTP_EPS ) 
		{
			if( ( rdParam < -0.5 ) || ( d_tmp < rdParam ) ) rdParam = d_tmp;
		}
//		std::cerr << rdParam << std::endl;

		return ( ( rdParam > -0.5 ) && ( rdParam <= 1.0 ) );
	}

	if( cd_r1 == 0.0 )
	{
		// intersection was in start point...
		return false;
	}

	// we have an intersection => find point
	const double	cd_v1x = ad_e1[ 0 ] - ad_s1[ 0 ];
	const double	cd_v1y = ad_e1[ 1 ] - ad_s1[ 1 ];
	const double	cd_v2x = ad_e2[ 0 ] - ad_s2[ 0 ];
	const double	cd_v2y = ad_e2[ 1 ] - ad_s2[ 1 ];
	const double	cd_ppx = ad_s2[ 0 ] - ad_s1[ 0 ];
	const double	cd_ppy = ad_s2[ 1 ] - ad_s1[ 1 ];
	const double	cd_a = cd_ppx * cd_v2y - cd_ppy * cd_v2x;
	const double	cd_b = cd_v1x * cd_v2y - cd_v1y * cd_v2x;

	rdParam = cd_a / cd_b;
//	std::cerr << cd_a << " / " << cd_b << " = " << rdParam << std::endl;

	if( rdParam <= 0.0 )
	{
		rdParam = 0.0;
		// intersection was in start point...
//		std::cerr << "int at start " << cd_r1 << std::endl;
//		return false;
	}
	if( rdParam >= 1.0 )
	{
		rdParam = 1.0;
	}
	return true;
}

void ParSpaceTrimmer::deleteVertexInfo( )
{
	dctpvertexset::iterator	itspcl_end;
	dctpvertexset::iterator	itspcl_v;

	itspcl_end = mesh->vertices.end( );

	for( itspcl_v = mesh->vertices.begin( ); itspcl_v != itspcl_end; ++itspcl_v )
	{
		if( ( *itspcl_v )->vertexinfo )
		{
#ifdef OSG_FORCE_NO_T_VERTICES
			delete ( SPosNorm* ) ( *itspcl_v )->vertexinfo;
#else
			delete ( vec3d* ) ( *itspcl_v )->vertexinfo;
#endif
			( *itspcl_v )->vertexinfo = NULL;
		}
	}
}


bool ParSpaceTrimmer::isLoopValid( const unsigned int cuiLoop )
{
	const unsigned int	cui_loop_cnt = ( *pvccrd ).size( );
	unsigned int		ui_loop;
	unsigned int		ui_edge_cnt;
	unsigned int		ui_vertex_cnt;
	unsigned int		ui_vertex;
//	SimplePolygon		cl_check;
	vec2d				cl_ray_start;
	vec2d				cl_ray_end;

	ui_vertex_cnt = ( *pvccrd )[ cuiLoop ].size( );
/*	cl_check.vertices.resize( ui_vertex_cnt );
	for( ui_vertex = 0; ui_vertex < ui_vertex_cnt; ++ui_vertex )
	{
		cl_check.vertices[ ui_vertex ] = ui_vertex;
	}
	if( cl_check.isReversed( ( *pvccrd )[ cuiLoop ] ) )
	{
//		std::cerr << "loop is reversed" << std::endl;
		ui_edge_cnt = 1;
		m_vbReversed[ cuiLoop ] = true;
	}
	else
	{
//		std::cerr << "loop is normal" << std::endl;
		ui_edge_cnt = 0;
		m_vbReversed[ cuiLoop ] = false;
	}*/

	if( ( *m_pvbReversed )[ cuiLoop ] )
	{
		ui_edge_cnt = 1;
	}
	else
	{
		ui_edge_cnt = 0;
	}

	cl_ray_start = ( ( *pvccrd )[ cuiLoop ][ 0 ] + ( *pvccrd )[ cuiLoop ][ 1 ] ) * 0.5;
//	cl_ray_end.x = -DBL_MAX;
	cl_ray_end.x = -FLT_MAX;
	cl_ray_end.y = cl_ray_start.y;

//	std::cerr << "ray: " << cl_ray_start << " -> " << cl_ray_end << std::endl;

	for( ui_loop = 0; ui_loop < cui_loop_cnt; ++ui_loop )
	{
		if( ui_loop != cuiLoop )
		{
			ui_vertex_cnt = ( *pvccrd )[ ui_loop ].size( );
			for( ui_vertex = 0; ui_vertex < ui_vertex_cnt; ++ui_vertex )
			{
//				std::cerr << "check: " << ( *pvccrd )[ ui_loop ][ ui_vertex ] << " -> " << ( *pvccrd )[ ui_loop ][ ( ui_vertex + 1 ) % ui_vertex_cnt ] << std::endl;
				if( intersectsRay( ( *pvccrd )[ ui_loop ][ ui_vertex ],
								   ( *pvccrd )[ ui_loop ][ ( ui_vertex + 1 ) % ui_vertex_cnt ],
								   cl_ray_start, cl_ray_end ) )
				{
//					std::cerr << "intersect" << std::endl;
					++ui_edge_cnt;
				}
			}
		}
	}

	if( ( ui_edge_cnt & 1 ) != 0 )
	{
//		std::cerr << "loop is removed" << std::endl;
	}

	return ( ( ui_edge_cnt & 1 ) == 0 );
}


bool ParSpaceTrimmer::intersectsRay( const vec2d cclV1, const vec2d cclV2, const vec2d cclStart, const vec2d cclEnd )
{
	double	ad_v1[ 2 ];
	double	ad_v2[ 2 ];
	double	ad_start[ 2 ];
	double	ad_end[ 2 ];

	if( cclV1 < cclV2 )
	{
		ad_v1[ 0 ] = cclV1.x;
		ad_v1[ 1 ] = cclV1.y;
		ad_v2[ 0 ] = cclV2.x;
		ad_v2[ 1 ] = cclV2.y;
	}
	else
	{
		ad_v1[ 0 ] = cclV2.x;
		ad_v1[ 1 ] = cclV2.y;
		ad_v2[ 0 ] = cclV1.x;
		ad_v2[ 1 ] = cclV1.y;
	}
	ad_start[ 0 ] = cclStart.x;
	ad_start[ 1 ] = cclStart.y;
	ad_end[ 0 ] = cclEnd.x;
	ad_end[ 1 ] = cclEnd.y;

	const double	cd_c1 = orient2d( ad_start, ad_end, ad_v1 );

	if( cd_c1 == 0.0 )
	{
		return false;	// not at start of edge
	}

	const double	cd_c2 = orient2d( ad_start, ad_end, ad_v2 );

	if( ( ( cd_c1 < 0.0 ) && ( cd_c2 < 0.0 ) ) || ( ( cd_c1 > 0.0 ) && ( cd_c2 > 0.0 ) ) )
	{
		return false;
	}

	const double	cd_c3 = orient2d ( ad_v1, ad_v2, ad_start );
	const double	cd_c4 = orient2d ( ad_v1, ad_v2, ad_end );

	if( ( ( cd_c3 < 0.0 ) && ( cd_c4 < 0.0 ) ) || ( ( cd_c3 > 0.0 ) && ( cd_c4 > 0.0 ) ) )
	{
		return false;
	}

	return true;
}


#ifdef OSG_USE_SIMPLIFIER
double ParSpaceTrimmer::DistToEdge( const vec3d cclPoint, const vec3d cclLine1, const vec3d cclLine2 ) const
{
	const vec3d		ccl_v = cclLine2 - cclLine1;
	const vec3d		ccl_w = cclPoint - cclLine1;

	const double	cd_c1 = ccl_v.x * ccl_w.x + ccl_v.y * ccl_w.y + ccl_v.z * ccl_w.z;

	if( cd_c1 <= 0.0 )
	{
		return ( cclPoint - cclLine1 ).quad_size( );
	}

	const double	cd_c2 = ccl_v.quad_size( );
	
	if( cd_c1 >= cd_c2 )
	{
		return ( cclPoint - cclLine2 ).quad_size( );
	}

	return ( cclPoint - ( cclLine1 + ccl_v * ( cd_c1 / cd_c2 ) ) ).quad_size( );
}
#endif
