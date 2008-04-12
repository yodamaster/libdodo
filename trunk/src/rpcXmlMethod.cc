/***************************************************************************
 *            rpcXmlMethod.cc
 *
 *  Wed Apr 09 23:30:55 2008
 *  Copyright  2008  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <libdodo/rpcXmlMethod.h>

using namespace dodo;

const char rpcXmlMethod::trimSymbols[] = {' ',
		'\r'
};

//-------------------------------------------------------------------

rpcMethod
rpcXmlMethod::xmlToRpcMethod(const dodoString &data)
{
	__xmlNodeDef xmlMethodCall;
	xmlMethodCall.name = "methodCall";
	xmlMethodCall.ignoreChildrenDef = true;
	
	xml xmlValue;

	__xmlNode node = xmlValue.parseBuffer(xmlMethodCall, data);
	
	return xmlToRpcMethod(node);
}

//-------------------------------------------------------------------

dodoString 
rpcXmlMethod::rpcMethodToXml(const rpcMethod &data)
{
	xml xmlValue;
	
	return xmlValue.createNode(rpcMethodToXmlNode(data));
}

//-------------------------------------------------------------------


rpcMethod
rpcXmlMethod::xmlToRpcMethod(__xmlNode &node)
{
	rpcMethod method;
	
	dodoMap<dodoString, dodoArray<__xmlNode>, dodoMapStringCompare>::iterator i = node.children.begin();
	if (stringTools::iequal(i->first, "methodName"))
		method.name = stringTools::trim(i->second[0].value, trimSymbols, 2);
	else
	{
		if (stringTools::iequal(i->first, "params"))
		{
			dodoArray<__xmlNode> &nodeArray = i->second[0].children["param"];
			
			dodoArray<__xmlNode>::iterator o = nodeArray.begin(), p = nodeArray.end();
			for (;o!=p;++o)
				method.arguments.push_back(rpcXmlValue::xmlToRpcValue(o->children["value"][0]));
		}
	}
	
	return method;
}

//-------------------------------------------------------------------

__xmlNode 
rpcXmlMethod::rpcMethodToXmlNode(const rpcMethod &data)
{
	dodoArray<__xmlNode> subNodeArr; 
	
	__xmlNode method;
	method.name = "methodCall";
	
	__xmlNode methodName;
	methodName.name = "methodName";
	methodName.value = data.name;
	
	subNodeArr.assign(1, methodName);
	method.children.insert(make_pair(methodName.name, subNodeArr));
	
	dodoArray<rpcValue>::const_iterator i = data.arguments.begin(), j = data.arguments.end();
	if (i!=j)
	{
		__xmlNode params;
		params.name = "params";
		
		__xmlNode param;
		param.name = "param";
		
		for (;i!=j;++i)
		{
			param.children.clear();
			
			subNodeArr.assign(1, rpcXmlValue::rpcValueToXmlNode(*i));
			param.children.insert(make_pair("value", subNodeArr));
		}

		subNodeArr.assign(1, params);
		method.children.insert(make_pair(params.name, subNodeArr));
	}
	
	return method;
}

//-------------------------------------------------------------------