/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "cmFindPathCommand.h"
#include "cmCacheManager.h"

// cmFindPathCommand
bool cmFindPathCommand::InitialPass(std::vector<std::string> const& argsIn)
{
  if(argsIn.size() < 2)
    {
    this->SetError("called with incorrect number of arguments");
    return false;
    }

  // Now check and see if the value has been stored in the cache
  // already, if so use that value and don't look for the program
  std::string helpString = "What is the path where the file ";
  helpString += argsIn[1] + " can be found";
  std::vector<std::string> args;
  unsigned int size = argsIn.size();
  for(unsigned int j = 0; j < size; ++j)
    {
    if(argsIn[j] != "DOC")
      {
      args.push_back(argsIn[j]);
      }
    else
      {
      if(j+1 < size)
        {
        helpString = argsIn[j+1];
        }
      break;
      }
    }
  const char* cacheValue
    = m_Makefile->GetDefinition(args[0].c_str());
  if(cacheValue && strcmp(cacheValue, "NOTFOUND"))
    { 
    return true;
    }

  std::vector<std::string> path;
  // add any user specified paths
  for (unsigned int j = 2; j < args.size(); j++)
    {
    // expand variables
    std::string exp = args[j];
    m_Makefile->ExpandVariablesInString(exp);
    cmSystemTools::ExpandRegistryValues(exp);      

    // Glob the entry in case of wildcards.
    cmSystemTools::GlobDirs(exp.c_str(), path);
    }

  // add the standard path
  cmSystemTools::GetPath(path);
  unsigned int k;
  for(k=0; k < path.size(); k++)
    {
    std::string tryPath = path[k];
    tryPath += "/";
    tryPath += args[1];
    if(cmSystemTools::FileExists(tryPath.c_str()))
      {
      path[k] = cmSystemTools::CollapseFullPath(path[k].c_str());
      m_Makefile->AddCacheDefinition(args[0].c_str(),
                                     path[k].c_str(),
                                     helpString.c_str(),
                                     cmCacheManager::PATH);
      return true;
      }
    }
  
  m_Makefile->AddCacheDefinition(args[0].c_str(),
                                 "NOTFOUND",
                                 helpString.c_str(),
                                 cmCacheManager::PATH);
  return true;
}

