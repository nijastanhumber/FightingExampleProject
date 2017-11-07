// Fill out your copyright notice in the Description page of Project Settings.

#include "FGMove.h"
#include "FGDefaultPawn.h"
#include "Engine/ActorChannel.h"

FFGMoveLinkToFollow UFGMove::TryLinks(const AFGDefaultPawn* RefObject, const TArray<USM_InputAtom*>& DataSource, int32 DataIndex /*= 0*/, int32 RemainingSteps /*= -1*/)
{
	FFGMoveLinkToFollow ResultLink;

	for (int32 i = 0; i < LinkedMoves.Num(); ++i)
	{
		check(LinkedMoves[i]);
		ResultLink.SMR = LinkedMoves[i]->TryLink(RefObject, DataSource, DataIndex, RemainingSteps);
		if (ResultLink.SMR.CompletionType == EStateMachineCompletionType::Accepted)
		{
			ResultLink.Link = LinkedMoves[i];
			return ResultLink;
		}
	}
	return ResultLink;
}

void UFGMove::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}


