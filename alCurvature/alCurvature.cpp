#include "Remap.h"
#include <ai.h>
#include <cassert>
#include <string>

AI_SHADER_NODE_EXPORT_METHODS(alCurvatureMtd)

#define MAX_SAMPLES 10
#define MAX_SAMPLES2 100

struct ShaderData
{
	AtSampler* sampler;
	std::string trace_set;
	int mode;
};

enum alCurvatureParams
{
	p_mode,
	p_samples,
	p_sampleRadius,
	p_traceSet,
	p_color1,
	p_color2,
	REMAP_FLOAT_PARAM_ENUM
};

AtRGB checker(AtPoint P, float f)
{
	if ((int(floor(P.x) + floor(P.y) + floor(P.z)) & 1) == 0)
    return AI_RGB_WHITE;
  else
    return AI_RGB_BLACK;
}

enum CurvatureMode
{
	CRV_POSITIVE=0,
	CRV_NEGATIVE
};

const char* curvatureModeNames[] = {
	"positive",
	"negative",
	NULL
};

node_parameters
{
	AiParameterENUM("mode", CRV_POSITIVE, curvatureModeNames)
	AiParameterINT("samples", 3);
	AiParameterFLT("sampleRadius", 1.0f);
	AiParameterSTR("traceSet", "");
	AiParameterRGB("color1", 0.0f, 0.0f, 0.0f);
	AiParameterRGB("color2", 1.0f, 1.0f, 1.0f);
	REMAP_FLOAT_PARAM_DECLARE;
}

node_loader
{
   if (i>0) return 0;
   node->methods     = alCurvatureMtd;
   node->output_type = AI_TYPE_RGB;
   node->name        = "alCurvature";
   node->node_type   = AI_NODE_SHADER;
   strcpy(node->version, AI_VERSION);
   return true;
}

node_initialize
{
	ShaderData *data = new ShaderData();
	AiNodeSetLocalData(node,data);
	data->sampler = NULL;
}

node_finish
{
	if (AiNodeGetLocalData(node))
	{
		ShaderData* data = (ShaderData*) AiNodeGetLocalData(node);
		AiSamplerDestroy(data->sampler);

		delete data;
		AiNodeSetLocalData(node, NULL);
	}
}

node_update
{
	ShaderData *data = (ShaderData*)AiNodeGetLocalData(node);
	AiSamplerDestroy(data->sampler);
	data->sampler = AiSampler(std::min(params[p_samples].INT, MAX_SAMPLES), 2);
	data->mode = params[p_mode].INT;
	data->trace_set = std::string();
	data->trace_set = params[p_traceSet].STR;
}

struct Sample
{
	float wi;
	AtPoint qi;
	AtVector ni;
};

shader_evaluate
{
	float result = 0.0f;
	ShaderData* data = (ShaderData*) AiNodeGetLocalData(node);
	float pa = AiV3Length(AiV3Cross(sg->dPdx, sg->dPdy));
	float sampleRadius = AiShaderEvalParamFlt(p_sampleRadius);// * sg->area * data->aas;
	AtRGB color1 = AiShaderEvalParamRGB(p_color1);
	AtRGB color2 = AiShaderEvalParamRGB(p_color2);
	
	// build a local frame for sampling
	AtVector U, V;
	AiBuildLocalFrameShirley(&U, &V, &sg->N);
	
	AtSamplerIterator* sampit = AiSamplerIterator(data->sampler, sg);
	AtRay ray;
	AtShaderGlobals* hitpoint = AiShaderGlobals();
   float samples[2];
	float du, dv;
	// AtVector dir = -sg->N;
	AtVector dir = sg->Rd;
	int count = 0;
	// float sampleRadius = sg->area * 10;
	float sampleOffset = sg->area * 10;
	AtUInt32 fi = sg->fi;
	sg->fi = UINT_MAX;

	AtPoint& pi = sg->P;
	float& t = sampleRadius;
	sampleOffset = sampleRadius * 0.5f;
	float t2 = t*t;
	
	if (data->trace_set.length())
	{
		AiShaderGlobalsSetTraceSet(sg, data->trace_set.c_str(), true);
	}

	Sample psamp[MAX_SAMPLES2];
	while (AiSamplerGetSample(sampit, samples))
	{
		// sample a disk above the normal to get the src points
		concentricSampleDisk(samples[0], samples[1], du, dv);
		// AtPoint srcpoint = sg->P + du*U*sampleRadius + dv*V*sampleRadius + sampleOffset*sg->N;
		AtPoint srcpoint = sg->P + du*U*sampleRadius + dv*V*sampleRadius - sampleOffset*sg->Rd;
		// AtPoint srcpoint = sg->P + sg->N*sampleOffset + uniformSampleSphere(samples[0], samples[1]);

		// trace straight back down
		AiMakeRay(&ray, AI_RAY_CAMERA, &srcpoint, &dir, sampleRadius, sg);
		AiTraceProbe(&ray, hitpoint);
		if (hitpoint)
		{
			AtVector L = hitpoint->P - sg->P;
			float dist2 = AiV3Dot(L, L);
			if (dist2 > t2) continue;
			Sample& s = psamp[count];
			// set qi to be relative to the shading point to avoid weirdness close to the origin
			// for that same reason move the whole calculation away from the origin
			s.qi = hitpoint->P - pi + AiVector(10,10,10);
			s.wi = SQR(dist2 / t2 - 1.0f);
			s.ni = hitpoint->N;
			assert(AiIsFinite(s.wi));
			count++;
		}		
	}

	AiShaderGlobalsUnsetTraceSet(sg);
	sg->fi = fi;

	if (count)
	{
		float wnorm = 0.0f;
		for (int i=0; i < count; ++i)
		{
			wnorm += psamp[i].wi;
		}
		wnorm = 1.0f / wnorm;

		float wi_qi_ni = 0.0f;
		AtVector wi_qi = AI_V3_ZERO;
		AtVector wn_qi = AI_V3_ZERO;
		AtVector wi_ni = AI_V3_ZERO;
		AtVector wn_ni = AI_V3_ZERO;
		float wi_qi_qi = 0.0f;
		for (int i=0; i < count; ++i)
		{
			Sample& s = psamp[i];

			float wn = s.wi * wnorm;
			wi_qi_ni += s.wi * AiV3Dot(s.qi, s.ni);
			wn_qi += wn * s.qi;
			wi_qi += s.wi * s.qi;
			wi_ni += s.wi * s.ni;
			wi_qi_qi += s.wi * AiV3Dot(s.qi, s.qi);
			wn_ni += wn * s.ni;
		}

		float uq_a = wi_qi_ni - AiV3Dot(wn_qi, wi_ni);
		float uq_b = wi_qi_qi - AiV3Dot(wn_qi, wi_qi);

		if (uq_b > 0.0f)
		{
			float uq = 0.5f * uq_a / uq_b;
			
			if (data->mode == CRV_POSITIVE)
			{
				result = MAX(uq, 0.0f);
			}
			else
			{
				result = MAX(-uq, 0.0f);
			}
			
			RemapFloat r = REMAP_FLOAT_CREATE;
			result = r.remap(result);
		}
	}

	sg->out.RGB = lerp(color1, color2, result);
	AiShaderGlobalsDestroy(hitpoint);	
}


