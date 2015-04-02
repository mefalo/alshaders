#! /usr/bin/env python


import os
import sys
import uuid
from string import maketrans

def enum(*sequential, **named):
   enums = dict(zip(sequential, range(len(sequential))), **named)
   reverse = dict((value, key) for key, value in enums.iteritems())
   enums['reverse_mapping'] = reverse
   return type('Enum', (), enums)

class UiElement:
   name = ''
   ident = ''
   description = None
   parent = None
   children = None


class Group(UiElement):
   collapse = True
   

   def __init__(self, name, collapse=True, description=None, ident=None):
      self.name = name
      self.collapse = collapse
      self.description = description
      
      if ident==None:
         ident = name.replace(' ', '')
         ident = ident.replace('_', '')
         ident = ident.lower()
         temptrans = maketrans('','')
         self.ident = ident.translate(temptrans,'aeiouy')
         
      else:
         self.ident = ident

      self.unique_name = None

   def __str__(self):
      return 'Group %s' % self.name


class Tab(Group):
   def __init__(self, name, collapse=True, description=None,  ident=None):
      self.name = name
      self.collapse = collapse
      self.description = description
      
      if ident==None:
         ident = name.replace(' ', '')
         ident = ident.replace('_', '')
         ident = ident.lower()
         temptrans = maketrans('','')
         self.ident = ident.translate(temptrans,'aeiouy')
      else:
         self.ident = ident

      self.unique_name = None

   def __str__(self):
      return 'Tab %s' % self.name

class Parameter(UiElement):
   ptype = None
   label = None
   mn = None
   mx = None
   smn = None
   smx = None
   connectible = True
   enum_names = None
   default = None

   def __init__(self, name, ptype, default, label=None, description=None, mn=None, mx=None, smn=None, smx=None, connectible=True, enum_names=None):
      self.name = name
      self.ptype = ptype
      self.description = description      
      if not label:
         label = name   
      self.label = label   
      self.mn = mn
      self.mx = mx
      self.smn = smn
      self.smx = smx

      if ptype not in ('bool', 'int', 'float', 'rgb', 'vector', 'string', 'enum', 'matrix'):
         raise ValueError('parameter %s has unrecognized ptype %r' % (name, ptype))

      if ptype == 'enum':
         self.enum_names = enum_names

      if ptype == 'bool' or ptype == 'string' or ptype == 'int' or ptype == 'enum':
         self.connectible = False
      else:
         self.connectible = connectible

      # do a quick sanity check on the ptype and default
      if ptype == 'bool' and type(default) is not bool:
         raise ValueError('parameter %s was typed as %s but default had type %r' % (name, ptype, type(default)))
      elif ptype in ('rgb', 'vector') and type(default) is not tuple:
         raise ValueError('parameter %s was typed as %s but default had type %r' % (name, ptype, type(default)))
      elif ptype in ('string', 'enum') and type(default) is not str:
         raise ValueError('parameter %s was typed as %s but default had type %r' % (name, ptype, type(default)))

      self.default = default

   def __str__(self):
      return 'Parameter %s %s' % (self.name, self.ptype)

class AOV(Parameter):
   def __init__(self, name, ptype, label=None):
      self.name = name
      self.ptype = ptype
      self.label = label
      self.connectible = False

   def __str__(self):
      return 'AOV %s %s' % (self.name, self.ptype)

class ShaderDef:
   name = None
   description = None
   output = None
   c4d_classification = None
   c4d_menu = None
   c4d_command_id = None
   maya_name = None
   maya_classification = None
   maya_id = None
   maya_swatch = False
   maya_bump = False
   maya_matte = False
   soft_name = None
   soft_classification = None
   help_url = None
   soft_version = 1
   hierarchy_depth = 0
   root = None
   current_parent = None
   parameters = []
   aovs = []
   groups = []
   tabs = []

   def __init__(self):
      self.root = Group('__ROOT__', False)
      self.current_parent = self.root

   def setMayaClassification(self, mc):
      self.maya_classification = mc

   def setDescription(self, d):
      self.description = d

   def setOutput(self, o):
      self.output = o

   def beginGroup(self, name, collapse=True, description='', ident=None):
      g = None
      if self.current_parent is self.root:
         g = Tab(name, collapse, description,ident)
      else:
         g = Group(name, collapse, description,ident)

      if not self.current_parent.children:
         self.current_parent.children = [g]
      else:
         self.current_parent.children.append(g)

      g.parent = self.current_parent
      self.current_parent = g

   def endGroup(self):
      self.current_parent = self.current_parent.parent

   def parameter(self, name, ptype, default, label=None, description=None, mn=None, mx=None, smn=None, smx=None, connectible=True, enum_names=None):
      p = Parameter(name, ptype, default, label, description, mn, mx, smn, smx, connectible, enum_names)
      if not self.current_parent.children:
         self.current_parent.children = [p]
      else:
         self.current_parent.children.append(p)

      p.parent = self.current_parent
      self.parameters.append(p)

   def aov(self, name, ptype, label=None, description=None):
      p = AOV(name, ptype, label)
      if not self.current_parent.children:
         self.current_parent.children = [p]
      else:
         self.current_parent.children.append(p)

      p.parent = self.current_parent
      self.aovs.append(p)

   def __str__(self):
      return 'ShaderDef %s' % self.name

   def shader(self, d):
      self.name = d['name']
      self.description = d['description']
      self.output = d['output']
      self.c4d_classification = d.get('c4d_classification')
      self.c4d_menu = d.get('c4d_menu')
      self.c4d_command_id = d.get('c4d_command_id')
      self.maya_name = d['maya_name']
      self.maya_classification = d['maya_classification']
      self.maya_id = d['maya_id']
      self.maya_swatch = d['maya_swatch']
      self.maya_bump = d['maya_bump']
      self.maya_matte = d['maya_matte']
      self.soft_name = d['soft_name']
      self.soft_classification = d['soft_classification']
      self.soft_version = d['soft_version']
      if 'help_url' in d.keys():
         self.help_url = d['help_url']
      else:
         self.help_url = 'https://bitbucket.org/anderslanglands/alshaders/wiki/Home'


   #all groups need unique ident in houdini
   def uniqueGroupIdents(self):
      grpidents = []    
      for grp in self.groups:
         oldident=grp.ident
         if grp.ident in grpidents:
            grp.ident = '%s%d' % (grp.ident, grpidents.count(oldident))
         grpidents.append(oldident)    

class group:
   name = ''
   collapse = True
   description = ''
   ui = None
   ident = None

   def __init__(self, ui, name, collapse=True, description=None, ident=None):
      self.ui = ui
      self.name = name     
      self.collapse = collapse
      self.description = description
      self.ident = ident

   def __enter__(self):
      self.ui.beginGroup(self.name, self.collapse, self.description, self.ident)

   def __exit__(self, type, value, traceback):
      self.ui.endGroup()


def DebugPrintElement(el, d):
   indent = ''
   for i in range(d):
      indent += '\t'
   print '%s %s' % (indent, el)
   if isinstance(el, Group) and el.children:
      print '%s has %d children' % (el, len(el.children))
      for e in el.children:
         DebugPrintElement(e, d+1)
      d -= 1

def DebugPrintShaderDef(sd):
   print '%s' % sd
   DebugPrintElement(sd.root, 0)

def writei(f, s, d=0):
   indent = ''
   for i in range(d):
      indent += '\t'
   f.write('%s%s\n' %(indent, s))

def WalkAETemplate(el, f, d):
   if isinstance(el, Group):
      writei(f, 'self.beginLayout("%s", collapse=%r)' % (el.name, el.collapse), d)

      if el.children:
         for e in el.children:
            WalkAETemplate(e, f, d)

      writei(f, 'self.endLayout() # END %s' % el.name, d)

   elif isinstance(el, Parameter):
      writei(f, 'self.addControl("%s", label="%s")' % (el.name, el.label), d)

def WriteAETemplate(sd, fn):
   f = open(fn, 'w')
   writei(f, 'import pymel.core as pm', 0)
   writei(f, 'from alShaders import alShadersTemplate\n', 0)

   writei(f, 'class AE%sTemplate(alShadersTemplate):' % sd.name, 0)
   writei(f, 'def setup(self):', 1)

   if sd.maya_swatch:
      writei(f, 'self.addSwatch()', 2)

   writei(f, 'self.beginScrollLayout()', 2) # begin main scrollLayout
   writei(f, '')

   if sd.maya_matte:
      writei(f, 'self.beginLayout("Matte")', 2)
      writei(f, 'self.addControl("aiEnableMatte", label="Enable Matte")', 2)
      writei(f, 'self.addControl("aiMatteColor", label="Matte Color")', 2)
      writei(f, 'self.addControl("aiMatteColorA", label="Matte Opacity")', 2)
      writei(f, 'self.endLayout()', 2)


   for e in sd.root.children:
      WalkAETemplate(e, f, 2)

   if sd.maya_bump:
      writei(f, 'self.addBumpLayout()', 2)      

   writei(f, '')
   writei(f, 'pm.mel.AEdependNodeTemplate(self.nodeName)', 2)

   writei(f, 'self.addExtraControls()', 2)

   writei(f, '')
   writei(f, 'self.endScrollLayout()', 2) #end main scrollLayout

   f.close()

#########
# C4DtoA
#########

# Parameter ids are generated from the name.
def GenerateC4DtoAId(name, parameter_name):
   unique_name = "%s.%s" % (name, parameter_name)

   pid = 5381
   for c in unique_name:
      pid = ((pid << 5) + pid) + ord(c) # hash*33 + c
      #pid = pid*33 + ord(c)
      #print "%d %c" % (pid, c)

   # convert to unsigned int
   pid = pid & 0xffffffff
   if pid > 2147483647: pid = 2*2147483647 - pid + 2

   return pid

# Writes out group ids to the header file.
def WalkC4DtoAHeaderGroups(el, f, name):
   if isinstance(el, Group):
      group_name = el.unique_name if el.unique_name else el.name
      writei(f, 'C4DAI_%s_%s_GRP,' % (name.upper(), group_name.upper().replace(' ', '_')), 1)

      if el.children:
         for e in el.children:
            WalkC4DtoAHeaderGroups(e, f, name)

# Writes out parameter ids to the header file.
def WalkC4DtoAHeaderParameters(el, f, name):
   if isinstance(el, Group):
      if el.children:
         for e in el.children:
            WalkC4DtoAHeaderParameters(e, f, name)

   # skip aovs
   #if isinstance(el, AOV):
   #  pass

   if isinstance(el, Parameter):
      pid = GenerateC4DtoAId(name, el.name)
      writei(f, 'C4DAIP_%s_%s = %d,' % (name.upper(), el.name.upper().replace(' ', '_'), pid), 1)

# Writes .h header file.
def WriteC4DtoAHeaderFile(sd, name, build_dir):
   path = os.path.join(build_dir, "C4DtoA", "res", "description", "ainode_%s.h" % name)
   if os.path.exists(path):
      os.remove(path)
   if not os.path.exists(os.path.dirname(path)):
      os.makedirs(os.path.dirname(path))
   f = open(path, 'w')

   writei(f, '#ifndef _ainode_%s_h_' % name, 0)
   writei(f, '#define _ainode_%s_h_' % name, 0) 
   writei(f, '', 0) 
   writei(f, 'enum', 0) 
   writei(f, '{', 0) 
   writei(f, 'C4DAI_%s_MAIN_GRP = 2001,' % (name.upper()), 1)

   # groups
   for e in sd.root.children:
      WalkC4DtoAHeaderGroups(e, f, name)
   # matte
   if sd.maya_matte:
      writei(f, 'C4DAI_%s_MATTE_GRP,' % name.upper(), 1)   

   writei(f, '', 0)

   # parameters
   for e in sd.root.children:
      WalkC4DtoAHeaderParameters(e, f, name)
   # matte
   if sd.maya_matte:
      writei(f, 'C4DAIP_%s_AIENABLEMATTE = %d,' % (name.upper(), GenerateC4DtoAId(name, 'aiEnableMatte')), 1)
      writei(f, 'C4DAIP_%s_AIMATTECOLOR = %d,' % (name.upper(), GenerateC4DtoAId(name, 'aiMatteColor')), 1)
      writei(f, 'C4DAIP_%s_AIMATTECOLORA = %d,' % (name.upper(), GenerateC4DtoAId(name, 'aiMatteColorA')), 1)

   writei(f, '};', 0) 
   writei(f, '', 0) 
   writei(f, '#endif', 0) 
   writei(f, '', 0) 

# Writes out layout to the resource file.
def WalkC4DtoARes(el, f, name, d):
   if isinstance(el, Group):
      group_name = el.unique_name if el.unique_name else el.name      
      writei(f, 'GROUP C4DAI_%s_%s_GRP' % (name.upper(), group_name.upper().replace(' ', '_')), d)
      writei(f, '{', d)

      if not el.collapse:
         writei(f, 'DEFAULT 1;', d+1)
         writei(f, '', 0)

      if el.children:
         for e in el.children:
            WalkC4DtoARes(e, f, name, d+1)

      writei(f, '}', d)
      writei(f, '', 0)

   # skip aovs
   #elif isinstance(el, AOV):
   #  pass

   elif isinstance(el, Parameter):
      writei(f, 'AIPARAM C4DAIP_%s_%s {}' % (name.upper(), el.name.upper().replace(' ', '_')), d)

# Writes .res resource file.
def WriteC4DtoAResFile(sd, name, build_dir):
   path = os.path.join(build_dir, "C4DtoA", "res", "description", "ainode_%s.res" % name)
   if os.path.exists(path):
      os.remove(path)
   if not os.path.exists(os.path.dirname(path)):
      os.makedirs(os.path.dirname(path))
   f = open(path, 'w')

   writei(f, 'CONTAINER AINODE_%s' % name.upper(), 0)
   writei(f, '{', 0)
   writei(f, 'NAME ainode_%s;' % name, 1)
   writei(f, '', 0)
   writei(f, 'INCLUDE GVbase;', 1)
   writei(f, '', 0)
   writei(f, 'GROUP C4DAI_%s_MAIN_GRP' % name.upper(), 1)
   writei(f, '{', 1)
   writei(f, 'DEFAULT 1;', 2)
   writei(f, '', 0)

   # matte
   if sd.maya_matte:
      writei(f, 'GROUP C4DAI_%s_MATTE_GRP' % name.upper(), 2)
      writei(f, '{', 2)
      writei(f, 'AIPARAM C4DAIP_%s_AIENABLEMATTE {}' % name.upper(), 3)
      writei(f, 'AIPARAM C4DAIP_%s_AIMATTECOLOR {}' % name.upper(), 3)
      writei(f, 'AIPARAM C4DAIP_%s_AIMATTECOLORA {}' % name.upper(), 3)
      writei(f, '}', 2)
      writei(f, '', 0)

   # groups and parameters
   for e in sd.root.children:
      WalkC4DtoARes(e, f, name, 2)

   writei(f, '}', 1)
   writei(f, '}', 0)
   writei(f, '') 

# Writes out group labels to the string file.
def WalkC4DtoAStringGroups(el, f, name):
   if isinstance(el, Group):
      group_name = el.unique_name if el.unique_name else el.name
      writei(f, 'C4DAI_%s_%s_GRP   "%s";' % (name.upper(), group_name.upper().replace(' ', '_'), el.name), 1)

      if el.children:
         for e in el.children:
            WalkC4DtoAStringGroups(e, f, name)

# Writes out parameter labels to the string file.
def WalkC4DtoAStringParameters(el, f, name):
   if isinstance(el, Group):
      if el.children:
         for e in el.children:
            WalkC4DtoAStringParameters(e, f, name)

   if isinstance(el, Parameter) and el.label:
      writei(f, 'C4DAIP_%s_%s   "%s";' % (name.upper(), el.name.upper(), el.label), 1)

# Writes .str string file.
def WriteC4DtoAStringFile(sd, name, build_dir):
   path = os.path.join(build_dir, "C4DtoA", "res", "strings_us", "description", "ainode_%s.str" % name)
   if os.path.exists(path):
      os.remove(path)
   if not os.path.exists(os.path.dirname(path)):
      os.makedirs(os.path.dirname(path))
   f = open(path, 'w')

   writei(f, 'STRINGTABLE ainode_%s' % name, 0)
   writei(f, '{', 0)
   writei(f, 'ainode_%s   "Arnold %s node";' % (name, name), 1) 
   writei(f, '', 0)
   writei(f, 'C4DAI_%s_MAIN_GRP   "Main";' % (name.upper()), 1) 

   # groups
   for e in sd.root.children:
      WalkC4DtoAStringGroups(e, f, name)
   # matte
   if sd.maya_matte:
      writei(f, 'C4DAI_%s_MATTE_GRP   "Matte";' % name.upper(), 1)

   writei(f, '', 0)

   # parameters
   for e in sd.root.children:
      WalkC4DtoAStringParameters(e, f, name)
   # matte
   if sd.maya_matte:
      writei(f, 'C4DAIP_%s_AIENABLEMATTE   "Enable matte";' % name.upper(), 1)
      writei(f, 'C4DAIP_%s_AIMATTECOLOR   "Matte color";' % name.upper(), 1)
      writei(f, 'C4DAIP_%s_AIMATTECOLORA   "Matte opacity";' % name.upper(), 1)

   writei(f, '}', 0)
   writei(f, '', 0)

# Writes resource files for C4DtoA.
# To describe the UI we need three resource files:
#  - .h header file which contains parameter ids
#  - .res resource file which contains widget layout
#  - .str string file which contains labels
def WriteC4DtoAResourceFiles(sd, name, build_dir):
   # NOTE group list is build in WriteMTD
   # create unique group names
   names = set()
   groups = sd.tabs[:]
   groups.extend(sd.groups)
   for group in groups:
      if group.name not in names:
         names.add(group.name)
         continue

      i = 1
      group.unique_name = group.name
      while group.unique_name in names:
         group.unique_name = group.name + " %d" % i
         i += 1
      names.add(group.unique_name)

   # header file
   WriteC4DtoAHeaderFile(sd, name, build_dir)
   # resource file
   WriteC4DtoAResFile(sd, name, build_dir)
   # string file
   WriteC4DtoAStringFile(sd, name, build_dir)


#make list of groups and tabs
def buildGroupList(sd, el):   
   if isinstance(el, Group):
      if not isinstance(el, Tab):      
         sd.groups.append(el)

   if isinstance(el, Tab):    
      sd.tabs.append(el)

   if el.children:
      for e in el.children:
         buildGroupList(sd, e)
   

#find the total number of child groups and parameters of a Tab
def findTotalChildGroups(tab):
   if tab.children:
      totalchildren=len(tab.children)  
   
      for t in tab.children:
         totalchildren+=findTotalChildGroups(t)

      return totalchildren
   else:
      return 0 


#print ordered list of all parameters and groups   
def writeParameterOrder(f, grp, first=1, orderc=1):
   firstfolder=first
   ordercount=orderc
   for gr in grp.children:
      if isinstance(gr, Parameter):
         f.write('%s ' % gr.name)

      if isinstance(gr, Group) and not isinstance(gr, Tab):
         f.write('%s ' % gr.ident)
         writeParameterOrder(f,gr,firstfolder,ordercount)
      
      if isinstance(gr, Tab):
         if firstfolder == 1:
            f.write('folder1 ')
            firstfolder=0
         ordercount+=1
         f.write('"\n\thoudini.order%d STRING "' % ordercount)
      
         writeParameterOrder(f,gr, firstfolder,ordercount)  
            

def WriteMDTHeader(sd, f): 
   writei(f, '[node %s]' % sd.name, 0)
   writei(f, 'desc STRING "%s"' % sd.description, 1)
   if sd.c4d_classification: writei(f, 'c4d.classification STRING "%s"' % sd.c4d_classification, 1)
   if sd.c4d_menu: writei(f, 'c4d.menu STRING "%s"' % sd.c4d_menu, 1)
   if sd.c4d_command_id: writei(f, 'c4d.command_id INT %s' % sd.c4d_command_id, 1)
   writei(f, 'maya.name STRING "%s"' % sd.name, 1)
   writei(f, 'maya.classification STRING "%s"' % sd.maya_classification, 1)
   writei(f, 'maya.id INT %s' % sd.maya_id, 1)
   #writei(f, 'houdini.label STRING "%s"' % sd.name, 1)
   #writei(f, 'houdini.icon STRING "SHOP_surface"', 1)
   writei(f, 'houdini.help_url STRING "%s"' % sd.help_url ,1)

            
   #print tabs and numchildren to folder array
   if len(sd.tabs)>0:
      f.write('\thoudini.parm.folder.folder1 STRING "')  
      
      for t in range(len(sd.tabs)):       
         totalchildren= findTotalChildGroups(sd.tabs[t])       

         if t<len(sd.tabs)-1:
            f.write("%s;%d;" % (sd.tabs[t].name, totalchildren))
         else: 
            f.write("%s;%d" % (sd.tabs[t].name, totalchildren))   
      f.write('"\n')

   #print all groups
   for g in sd.groups:
      writei(f, 'houdini.parm.heading.%s STRING "%s"' % (g.ident, g.name),1)
   

   #print parameter order
   f.write('\thoudini.order STRING "')
   writeParameterOrder(f,sd.root,1,1)

   for av in sd.aovs:
      f.write('%s ' % av.name)

   f.write('"\n') 

   
def WriteMTDParam(f, name, ptype, value, d):
   if value == None:
      return

   if ptype == 'bool':
      if value:
         bval = "TRUE"
      else:
         bval = "FALSE"
      writei(f, '%s BOOL %s' % (name, bval), d)
   elif ptype == 'int':
      writei(f, '%s INT %d' % (name, value), d)
   elif ptype == 'float':
      writei(f, '%s FLOAT %r' % (name, value), d)
   elif ptype == 'string':
      writei(f, '%s STRING "%r"' % (name, value), d)

def WriteMTD(sd, fn):      
   
   #build tab and group lists
   for e in sd.root.children:
      buildGroupList(sd, e)
   
   #make all groupident unique
   sd.uniqueGroupIdents()  

   f = open(fn, 'w') 
   
   WriteMDTHeader(sd, f)   

   writei(f, '')

   for p in sd.parameters:
      writei(f, '[attr %s]' % p.name, 1)
      writei(f, 'houdini.label STRING "%s"' % p.label, 2)
      WriteMTDParam(f, "min", "float", p.mn, 2)
      WriteMTDParam(f, "max", "float", p.mx, 2)
      WriteMTDParam(f, "softmin", "float", p.smn, 2)
      WriteMTDParam(f, "softmax", "float", p.smx, 2)               
      WriteMTDParam(f, "desc", "string", p.description, 2)
      WriteMTDParam(f, "linkable", "bool", p.connectible, 2)

   for a in sd.aovs:
      writei(f, '[attr %s]' % a.name, 1)
      writei(f, 'houdini.label STRING "%s"' % a.label, 2)
      if (a.ptype == 'rgb'):
         writei(f, 'aov.type INT 0x05', 2)
      elif (a.ptype == 'rgba'):
         writei(f, 'aov.type INT 0x06', 2)
      writei(f, 'aov.enable_composition BOOL TRUE', 2)               
      writei(f, 'default STRING "%s"' % a.name[4:], 2)

   f.close()

def WalkArgs(el, f, d):
   if isinstance(el, Group):
      writei(f, '<page name="%s">' % (el.name), d)

      if el.children:
         for e in el.children:
            WalkArgs(e, f, d+1)

      writei(f, '</page>', d)
   elif isinstance(el, AOV):
      writei(f, '<param name="%s" label="%s"/>' % (el.name, el.label), d)
   elif isinstance(el, Parameter):
      if el.ptype == 'bool':
         writei(f, '<param name="%s" label="%s" widget="checkBox"/>' % (el.name, el.label), d)
      elif el.ptype == 'int':
         writei(f, '<param name="%s" label="%s" int="True"/>' % (el.name, el.label), d)
      elif el.ptype == 'rgb':
         writei(f, '<param name="%s" label="%s" widget="color"/>' % (el.name, el.label), d)
      elif el.ptype == 'enum':
         writei(f, '<param name="%s" label="%s" widget="popup">' % (el.name, el.label), d)
         
         writei(f, '<hintlist name="options">', d+1)
         
         for en in el.enum_names:
            writei(f, '<string value="%s"/>' % en, d+2)  

         writei(f, '</hintlist>', d+1)
         writei(f, '</param>', d)
      else:
         writei(f, '<param name="%s" label="%s"/>' % (el.name, el.label), d)


def WriteArgs(sd, fn):
   f = open(fn, 'w')
   writei(f, '<args format="1.0">', 0)

   for e in sd.root.children:
      WalkArgs(e, f, 0)

   writei(f, '</args>', 0)

def getSPDLTypeName(t):
   if t == 'bool':
      return 'boolean'
   elif t == 'int':
      return 'integer'
   elif t == 'float':
      return 'scalar'
   elif t == 'rgb':
      return 'color'
   elif t == 'enum':
      return 'string'
   else:
      return t

def WriteSPDLParameter(f, p):
   writei(f, 'Parameter "%s" input' % p.name, 1)
   writei(f, '{', 1)
   writei(f, 'GUID = "{%s}";' % uuid.uuid4(), 2)
   if isinstance(p, AOV):
      writei(f, 'Type = string;', 2)
      si_aov_name = p.name.replace('aov_', 'arnold_').title()
      writei(f, 'Value = "%s";' % si_aov_name, 2)
   else:
      writei(f, 'Type = %s;' % getSPDLTypeName(p.ptype), 2)
      if p.connectible:
         writei(f, 'Texturable = on;', 2)
      else:
         writei(f, 'Texturable = off;', 2)
      if p.ptype == 'bool':
         writei(f, 'Value = %s;' % str(p.default).lower(), 2)
      elif p.ptype == 'int':
         writei(f, 'Value = %d;' % p.default, 2)
      elif p.ptype == 'float':
         writei(f, 'Value = %f;' % p.default, 2)
      elif p.ptype == 'rgb' or p.ptype == 'vector':
         writei(f, 'Value = %f %f %f;' % p.default, 2)
      elif p.ptype == 'string':
         writei(f, 'Value = "%s";' % p.default, 2)

   writei(f, '}', 1)

def WriteSPDLPropertySet(f, sd):
   writei(f, 'PropertySet "%s_pset"' % sd.name)
   writei(f, '{')


   writei(f, 'Parameter "out" output', 1)
   writei(f, '{', 1)
   writei(f, 'GUID = "{%s}";' % uuid.uuid4(), 2)
   writei(f, 'Type = %s;' % getSPDLTypeName(sd.output), 2)
   writei(f, '}', 1)

   for p in sd.parameters:
      WriteSPDLParameter(f, p)

   for a in sd.aovs:
      WriteSPDLParameter(f, a)

   writei(f, '}')

def writeSPDLDefault(f, p, i):
   writei(f, '%s' % p.name, 1)
   writei(f, '{', 1)

   writei(f, 'Name = "%s";' % p.label, 2)
   if p.ptype == 'rgb':
      writei(f, 'UIType = "rgb";', 2)
   elif p.ptype=='enum':
      writei(f, 'UIType = "Combo";', 2)
      writei(f, 'Items', 2)
      writei(f, '{', 2)
      for ev in p.enum_names:
         writei(f, '"%s" = "%s";' % (ev, ev), 3)
      writei(f, '}', 2)
   elif p.smn and p.smx:
      writei(f, 'UIRange = %f To %f;' % (p.smn, p.smx), 2)


   if p.connectible:
      writei(f, 'Commands = "{F5C75F11-2F05-11D3-AA95-00AA0068D2C0}";', 2)

   writei(f, '}', 1)

def WalkSPDLDefault(f, el, d):

   if isinstance(el, Group):
      if isinstance(el, Tab):
         writei(f, 'Tab "%s"' % el.name, d)
      else:
         writei(f, 'Group "%s"' % el.name, d)
      writei(f, '{', d)
      if el.children:
         for e in el.children:
            WalkSPDLDefault(f, e, d+1)
      writei(f, '}', d)

   elif isinstance(el, Parameter):
      writei(f, '%s;' % el.name, d)

def WalkSPDLTestChildren(el):
   childrenHaveMembers = False
   if el.children:
      for e in el.children:
         if isinstance(e, Group):
            childrenHaveMembers = childrenHaveMembers or WalkSPDLTestChildren(e)
         elif isinstance(e, Parameter):
            childrenHaveMembers = childrenHaveMembers or e.connectible
   return childrenHaveMembers

def WalkSPDLRender(f, el, d):
   if isinstance(el, Group) and WalkSPDLTestChildren(el):
      writei(f, 'Group "%s"' % el.name, d)
      writei(f, '{', d)
      if el.children:
         for e in el.children:
            WalkSPDLRender(f, e, d+1)
      writei(f, '}', d)

   elif isinstance(el, Parameter) and el.connectible:
      writei(f, '%s;' % el.name, d)
      

def WriteSPDL(sd, fn):
   f = open(fn, 'w')
   writei(f, 'SPDL')
   writei(f, 'Version = "2.0.0.0";')
   writei(f, 'Reference = "{%s}";' % uuid.uuid4())

   WriteSPDLPropertySet(f, sd)

   writei(f, 'MetaShader "%s_meta"' % sd.name)
   writei(f, '{')
   writei(f, 'Name = "%s";' % sd.soft_name, 1)
   writei(f, 'Type = %s;' % sd.soft_classification, 1)
   writei(f, 'Renderer "mental ray"', 1)
   writei(f, '{', 1)
   writei(f, 'Name = "%s";' % sd.soft_name, 2)
   writei(f, 'Filename = "%s";' % sd.name, 2)
   writei(f, 'Options', 2)
   writei(f, '{', 2)
   writei(f, '"version" = 1;', 3)
   writei(f, '}', 2)
   writei(f, '}', 1)
   writei(f, '}')

   # Begin Defaults
   writei(f, 'Defaults')
   writei(f, '{')

   cmd = uuid.uuid4()

   for p in sd.parameters:
      writeSPDLDefault(f, p, cmd)

   for a in sd.aovs:
      writei(f, '%s' % a.name, 1)
      writei(f, '{', 1)
      writei(f, 'Name = "%s";' % a.name, 2)
      writei(f, '}', 1)

   writei(f, '}')
   # End Defaults

   #Begin Layout
   writei(f, 'Layout "Default"')
   writei(f, '{')
   
   for el in sd.root.children:
      WalkSPDLDefault(f, el, 1)

   writei(f, '}')
   writei(f, 'Layout "RenderTree"')
   writei(f, '{')
   
   for el in sd.root.children:
      WalkSPDLRender(f, el, 1)

   writei(f, '}')
   # End Layout

   writei(f, 'Plugin = Shader')
   writei(f, '{')
   writei(f, 'Filename = "%s";' % sd.name, 1)
   writei(f, '}')

   f.close()

def remapControls(sd):
   with group(sd, 'Remap', collapse=True):
      sd.parameter('RMPinputMin', 'float', 0.0, label='Input min')
      sd.parameter('RMPinputMax', 'float', 1.0, label='Input max')

      with group(sd, 'Contrast', collapse=False):
         sd.parameter('RMPcontrast', 'float', 1.0, label='Contrast')
         sd.parameter('RMPcontrastPivot', 'float', 0.18, label='Pivot')

      with group(sd, 'Bias and gain', collapse=False):
         sd.parameter('RMPbias', 'float', 0.5, label='Bias')
         sd.parameter('RMPgain', 'float', 0.5, label='Gain')

      sd.parameter('RMPoutputMin', 'float', 0.0, label='Output min')
      sd.parameter('RMPoutputMax', 'float', 1.0, label='Output max')

      with group(sd, 'Clamp', collapse=False):
         sd.parameter('RMPclampEnable', 'bool', False, label='Enable')
         sd.parameter('RMPthreshold', 'bool', False, label='Expand')
         sd.parameter('RMPclampMin', 'float', 0.0, label='Min')
         sd.parameter('RMPclampMax', 'float', 1.0, label='Max')

# Main. Load the UI file and build UI templates from the returned structure
if __name__ == '__main__':
   if len(sys.argv) < 6:
      print 'ERROR: must supply exactly ui source input and mtd, ae, spdl and args outputs'
      sys.exit(1)

   ui = ShaderDef()
   globals_dict = {'ui':ui}
   execfile(sys.argv[1], globals_dict)

   if not isinstance(ui, ShaderDef):
      print 'ERROR: ui object is not a ShaderDef. Did you assign something else to it by mistake?'
      sys.exit(2)

   WriteMTD(ui, sys.argv[2])  
   WriteAETemplate(ui, sys.argv[3])
   WriteSPDL(ui, sys.argv[4])
   WriteArgs(ui, sys.argv[5])

   # C4DtoA resource files
   name = os.path.basename(os.path.splitext(sys.argv[1])[0])
   build_dir = sys.argv[6] if len(sys.argv) > 6 else os.path.abspath("")
   WriteC4DtoAResourceFiles(ui, name, build_dir)

