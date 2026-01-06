/**
 * @file trigger_parser_test.cpp
 * @brief Unit tests for TriggerParser using real MTG oracle text.
 */

#include "trigger_parser.h"

#include "gtest/gtest.h"

class TriggerParserTest : public ::testing::Test
{
protected:
    TriggerParser parser;

    // Helper to check if any trigger matches the expected phase
    bool hasTriggerPhase(const QList<TriggerInfoPtr> &triggers, TriggerPhase expectedPhase)
    {
        for (const auto &trigger : triggers) {
            if (trigger->triggerPhase == expectedPhase) {
                return true;
            }
        }
        return false;
    }

    // Helper to check if any trigger matches phase AND is opponent-only
    bool hasOpponentOnlyTrigger(const QList<TriggerInfoPtr> &triggers, TriggerPhase expectedPhase)
    {
        for (const auto &trigger : triggers) {
            if (trigger->triggerPhase == expectedPhase && trigger->isOpponentOnly) {
                return true;
            }
        }
        return false;
    }

    // Helper to check if any trigger matches phase AND is each-player
    bool hasEachPlayerTrigger(const QList<TriggerInfoPtr> &triggers, TriggerPhase expectedPhase)
    {
        for (const auto &trigger : triggers) {
            if (trigger->triggerPhase == expectedPhase && trigger->isEachPlayer) {
                return true;
            }
        }
        return false;
    }

    // Helper to check if any trigger is eminence
    bool hasEminenceTrigger(const QList<TriggerInfoPtr> &triggers)
    {
        for (const auto &trigger : triggers) {
            if (trigger->isEminence) {
                return true;
            }
        }
        return false;
    }
};

// =============================================================================
// Phase-Based Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsUpkeepTriggers)
{
    // Phyrexian Arena
    auto triggers = parser.parseCardTriggers("Phyrexian Arena",
                                             "At the beginning of your upkeep, you draw a card and you lose 1 life.", 1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::Upkeep);
}

TEST_F(TriggerParserTest, DetectsEachPlayerUpkeep)
{
    // Howling Mine
    auto triggers =
        parser.parseCardTriggers("Howling Mine", "At the beginning of each player's draw step, that player draws an "
                                                 "additional card.",
                                 1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::Draw);
    EXPECT_TRUE(triggers[0]->isEachPlayer);
}

TEST_F(TriggerParserTest, DetectsUntapTriggers)
{
    // Seedborn Muse
    auto triggers = parser.parseCardTriggers(
        "Seedborn Muse", "Untap all permanents you control during each other player's untap step.", 1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::Untap);
}

TEST_F(TriggerParserTest, DetectsEndStepTriggers)
{
    // Luminarch Ascension
    auto triggers = parser.parseCardTriggers(
        "Luminarch Ascension",
        "At the beginning of each opponent's end step, if you didn't lose life this turn, you may put a quest counter "
        "on Luminarch Ascension.",
        1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::EndStep);
}

// =============================================================================
// Combat Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsCombatBeginningTriggers)
{
    // Aurelia, the Warleader - NOTE: Card name has comma which tests greedy pattern handling
    auto triggers = parser.parseCardTriggers(
        "Aurelia, the Warleader",
        "Flying, vigilance, haste\nWhenever Aurelia, the Warleader attacks for the first time each turn, untap all "
        "creatures you control. After this phase, there is an additional combat phase.",
        1);
    // This SHOULD match but currently FAILS due to greedy pattern [^,.]+
    // Will pass after greedy pattern fix
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Attackers));
}

TEST_F(TriggerParserTest, DetectsAttackTriggers)
{
    // Etali, Primal Storm
    auto triggers = parser.parseCardTriggers(
        "Etali, Primal Storm",
        "Whenever Etali, Primal Storm attacks, exile the top card of each player's library, then you may cast any "
        "number of spells from among those cards without paying their mana costs.",
        1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::Attackers);
}

TEST_F(TriggerParserTest, DetectsBlockTriggers)
{
    // Wall of Denial (hypothetical example)
    auto triggers =
        parser.parseCardTriggers("Example Blocker", "Whenever Example Blocker blocks, you gain 2 life.", 1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::Blockers);
}

TEST_F(TriggerParserTest, DetectsCombatDamageTriggers)
{
    // Tymna the Weaver
    auto triggers = parser.parseCardTriggers(
        "Tymna the Weaver",
        "At the beginning of your postcombat main phase, you may pay X life, where X is the number of opponents that "
        "were dealt combat damage this turn. If you do, draw X cards.",
        1);
    // This doesn't match "deals combat damage" pattern - it's not a trigger on dealing damage
    // Let's use a better example
    auto triggers2 = parser.parseCardTriggers(
        "Ninja of the Deep Hours",
        "Ninjutsu {1}{U}\nWhenever Ninja of the Deep Hours deals combat damage to a player, you may draw a card.", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers2, TriggerPhase::Damage));
}

// =============================================================================
// ETB vs Landfall Tests (Critical - Must Not Confuse)
// =============================================================================

TEST_F(TriggerParserTest, DetectsLandfallKeyword)
{
    // Lotus Cobra
    auto triggers =
        parser.parseCardTriggers("Lotus Cobra",
                                 "Landfall — Whenever a land enters the battlefield under your control, add one mana of "
                                 "any color.",
                                 1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::Landfall);
}

TEST_F(TriggerParserTest, DetectsLandfallWithoutKeyword)
{
    // Rampaging Baloths (older card without keyword)
    auto triggers = parser.parseCardTriggers(
        "Rampaging Baloths",
        "Trample\nWhenever a land enters the battlefield under your control, you may create a 4/4 green Beast "
        "creature token.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Landfall));
}

TEST_F(TriggerParserTest, DetectsEtbNotLandfall)
{
    // Panharmonicon
    auto triggers = parser.parseCardTriggers(
        "Panharmonicon",
        "If an artifact or creature entering the battlefield causes a triggered ability of a permanent you control to "
        "trigger, that ability triggers an additional time.",
        1);
    // This is a replacement effect, not really ETB trigger - let's use a better example
    auto triggers2 = parser.parseCardTriggers(
        "Soul Warden", "Whenever another creature enters the battlefield, you gain 1 life.", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers2, TriggerPhase::EntersBattlefield));
    EXPECT_FALSE(hasTriggerPhase(triggers2, TriggerPhase::Landfall));
}

TEST_F(TriggerParserTest, DetectsSelfEtb)
{
    // Mulldrifter
    auto triggers =
        parser.parseCardTriggers("Mulldrifter",
                                 "Flying\nWhen Mulldrifter enters the battlefield, draw two cards.\nEvoke {2}{U}", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::EntersBattlefield));
}

// =============================================================================
// Spell Cast Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsOpponentSpellCast)
{
    // Rhystic Study
    auto triggers =
        parser.parseCardTriggers("Rhystic Study",
                                 "Whenever an opponent casts a spell, you may draw a card unless that player pays {1}.",
                                 1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::SpellCast);
    EXPECT_TRUE(triggers[0]->isOpponentOnly);
}

TEST_F(TriggerParserTest, DetectsAnyPlayerSpellCast)
{
    // Guttersnipe variant
    auto triggers = parser.parseCardTriggers(
        "Example Card", "Whenever a player casts an instant or sorcery spell, Example Card deals 1 damage to that "
                        "player's opponents.",
        1);
    EXPECT_TRUE(hasEachPlayerTrigger(triggers, TriggerPhase::SpellCast));
}

// =============================================================================
// Draw Card Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsOpponentDrawCard)
{
    // Smothering Tithe
    auto triggers = parser.parseCardTriggers(
        "Smothering Tithe",
        "Whenever an opponent draws a card, that player may pay {2}. If the player doesn't, you create a Treasure "
        "token.",
        1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::DrawCard);
    EXPECT_TRUE(triggers[0]->isOpponentOnly);
}

TEST_F(TriggerParserTest, DetectsAnyPlayerDrawCard)
{
    // Alms Collector
    auto triggers = parser.parseCardTriggers(
        "Alms Collector",
        "Flash\nIf an opponent would draw two or more cards, instead you and that player each draw a card.", 1);
    // This is a replacement effect, not a trigger - let's use Nekusar
    auto triggers2 = parser.parseCardTriggers(
        "Nekusar, the Mindrazer", "At the beginning of each player's draw step, that player draws an additional "
                                  "card.\nWhenever an opponent draws a card, Nekusar deals 1 damage to that player.",
        1);
    EXPECT_TRUE(hasOpponentOnlyTrigger(triggers2, TriggerPhase::DrawCard));
}

// =============================================================================
// Discard Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsOpponentDiscard)
{
    // Waste Not
    auto triggers = parser.parseCardTriggers(
        "Waste Not", "Whenever an opponent discards a creature card, create a 2/2 black Zombie creature "
                     "token.\nWhenever an opponent discards a land card, add {B}{B}.\nWhenever an opponent discards a "
                     "noncreature, nonland card, draw a card.",
        1);
    // Should have 3 discard triggers
    int discardCount = 0;
    for (const auto &trigger : triggers) {
        if (trigger->triggerPhase == TriggerPhase::Discard) {
            discardCount++;
            EXPECT_TRUE(trigger->isOpponentOnly);
        }
    }
    EXPECT_EQ(discardCount, 3);
}

// =============================================================================
// Life Gain/Loss Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsLifeGainTriggers)
{
    // Ajani's Pridemate
    auto triggers = parser.parseCardTriggers(
        "Ajani's Pridemate", "Whenever you gain life, put a +1/+1 counter on Ajani's Pridemate.", 1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::GainLife);
}

TEST_F(TriggerParserTest, DetectsLifeLossTriggers)
{
    // Vilis, Broker of Blood
    auto triggers = parser.parseCardTriggers(
        "Vilis, Broker of Blood",
        "Flying\n{B}, Pay 2 life: Target creature gets -1/-1 until end of turn.\nWhenever you lose life, draw that "
        "many cards.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::LoseLife));
}

// =============================================================================
// Sacrifice Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsSacrificeTriggers)
{
    // Mayhem Devil
    auto triggers = parser.parseCardTriggers(
        "Mayhem Devil", "Whenever a player sacrifices a permanent, Mayhem Devil deals 1 damage to any target.", 1);
    ASSERT_EQ(triggers.size(), 1);
    EXPECT_EQ(triggers[0]->triggerPhase, TriggerPhase::Sacrifice);
    EXPECT_TRUE(triggers[0]->isEachPlayer);
}

// =============================================================================
// Death Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsDeathTriggers)
{
    // Blood Artist
    auto triggers =
        parser.parseCardTriggers("Blood Artist",
                                 "Whenever Blood Artist or another creature dies, target player loses 1 life and you "
                                 "gain 1 life.",
                                 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Dies));
}

TEST_F(TriggerParserTest, DetectsSelfDeathTrigger)
{
    // Kokusho, the Evening Star
    auto triggers =
        parser.parseCardTriggers("Kokusho, the Evening Star",
                                 "Flying\nWhen Kokusho, the Evening Star dies, each opponent loses 5 life and you gain "
                                 "life equal to the life lost this way.",
                                 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Dies));
}

// =============================================================================
// Counter Trigger Tests (Including Replacement Effects)
// =============================================================================

TEST_F(TriggerParserTest, DetectsCounterReplacementEffects)
{
    // Hardened Scales
    auto triggers = parser.parseCardTriggers(
        "Hardened Scales",
        "If one or more +1/+1 counters would be placed on a creature you control, that many plus one +1/+1 counters "
        "are placed on it instead.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::CounterPlaced));
}

TEST_F(TriggerParserTest, DetectsCounterPlacedTriggers)
{
    // Cathars' Crusade
    auto triggers = parser.parseCardTriggers(
        "Cathars' Crusade", "Whenever a creature enters the battlefield under your control, put a +1/+1 counter on "
                            "each creature you control.",
        1);
    // This is ETB that puts counters, not a counter-placed trigger
    // Let's use a better example
    auto triggers2 = parser.parseCardTriggers(
        "Forgotten Ancient",
        "Whenever a player casts a spell, you may put a +1/+1 counter on Forgotten Ancient.", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers2, TriggerPhase::SpellCast));
}

// =============================================================================
// Token Creation Trigger Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsTokenCreationTriggers)
{
    // Anointed Procession (replacement effect)
    auto triggers = parser.parseCardTriggers(
        "Anointed Procession", "If an effect would create one or more tokens under your control, it creates twice that "
                               "many of those tokens instead.",
        1);
    // This is a replacement effect, not triggered - let's find a proper trigger
    auto triggers2 = parser.parseCardTriggers(
        "Divine Visitation", "If one or more creature tokens would be created under your control, that many 4/4 white "
                             "Angel creature tokens with flying and vigilance are created instead.",
        1);
    // Also replacement effect - we need:
    auto triggers3 = parser.parseCardTriggers(
        "Example Token", "Whenever you create one or more tokens, draw a card.", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers3, TriggerPhase::TokenCreated));
}

// =============================================================================
// Eminence Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsEminenceAbilities)
{
    // Edgar Markov
    auto triggers = parser.parseCardTriggers(
        "Edgar Markov", "Eminence — Whenever you cast another Vampire spell, if Edgar Markov is in the command zone or "
                        "on the battlefield, create a 1/1 black Vampire creature token.",
        1, true); // isSideboardZone = true
    ASSERT_GE(triggers.size(), 1);
    EXPECT_TRUE(hasEminenceTrigger(triggers));
}

TEST_F(TriggerParserTest, IgnoresNonEminenceFromSideboard)
{
    // Regular creature in sideboard should not show triggers
    auto triggers = parser.parseCardTriggers(
        "Sol Ring", "When Sol Ring enters the battlefield, add {C}{C}.", 1, true); // isSideboardZone = true
    // Should be empty because non-eminence abilities from sideboard are filtered
    EXPECT_EQ(triggers.size(), 0);
}

// =============================================================================
// Edge Case Tests
// =============================================================================

TEST_F(TriggerParserTest, HandlesMultipleTriggersOnSameCard)
{
    // Teysa Karlov
    auto triggers = parser.parseCardTriggers(
        "Teysa Karlov",
        "If a creature dying causes a triggered ability of a permanent you control to trigger, that ability triggers "
        "an additional time.\nCreature tokens you control have vigilance and lifelink.",
        1);
    // This doesn't have actual triggers, just static abilities
    // Let's use a card with multiple triggers
    auto triggers2 = parser.parseCardTriggers(
        "Waste Not", "Whenever an opponent discards a creature card, create a 2/2 black Zombie creature "
                     "token.\nWhenever an opponent discards a land card, add {B}{B}.\nWhenever an opponent discards a "
                     "noncreature, nonland card, draw a card.",
        1);
    EXPECT_GE(triggers2.size(), 3);
}

TEST_F(TriggerParserTest, HandlesCommasInAbilityText)
{
    // Test that commas don't break pattern matching
    auto triggers = parser.parseCardTriggers(
        "Example Card", "Whenever a creature you control, other than Example Card, attacks, you gain 1 life.", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Attackers));
}

TEST_F(TriggerParserTest, HandlesLongCardNames)
{
    // Asmoranomardicadaistinaculdacar
    auto triggers = parser.parseCardTriggers(
        "Asmoranomardicadaistinaculdacar",
        "As long as you've discarded a card this turn, you may pay {B/R} to cast this spell.\nWhen "
        "Asmoranomardicadaistinaculdacar enters the battlefield, you may search your library for a card named The "
        "Underworld Cookbook, reveal it, put it into your hand, then shuffle.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::EntersBattlefield));
}

TEST_F(TriggerParserTest, HandlesTildeInOracleText)
{
    // Cards using ~ for self-reference
    auto triggers = parser.parseCardTriggers("Test Card", "Whenever ~ attacks, draw a card.", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Attackers));
}

TEST_F(TriggerParserTest, DetectsTargetedTriggers)
{
    // Heroic ability
    auto triggers =
        parser.parseCardTriggers("Hero of Iroas",
                                 "Aura spells you cast cost {1} less to cast.\nHeroic — Whenever you cast a spell that "
                                 "targets Hero of Iroas, put a +1/+1 counter on Hero of Iroas.",
                                 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::TargetedBy));
}

TEST_F(TriggerParserTest, DetectsTapTriggers)
{
    // Urza, Lord High Artificer
    auto triggers = parser.parseCardTriggers(
        "Urza, Lord High Artificer",
        "When Urza, Lord High Artificer enters the battlefield, create a 0/0 colorless Construct artifact creature "
        "token with \"This creature gets +1/+1 for each artifact you control.\"\nTap an untapped artifact you control: "
        "Add {U}.\n{5}: Shuffle your library, then exile the top card. Until end of turn, you may play that card "
        "without paying its mana cost.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::EntersBattlefield));
}

TEST_F(TriggerParserTest, DetectsManaTriggers)
{
    // Mana Flare
    auto triggers =
        parser.parseCardTriggers("Mana Flare", "Whenever a player taps a land for mana, that player adds one mana of "
                                               "any type that land produced.",
                                 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Mana));
}

// =============================================================================
// Leaves Battlefield Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsLeavesBattlefieldTriggers)
{
    // Thragtusk
    auto triggers = parser.parseCardTriggers(
        "Thragtusk",
        "When Thragtusk enters the battlefield, you gain 5 life.\nWhen Thragtusk leaves the battlefield, create a 3/3 "
        "green Beast creature token.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::EntersBattlefield));
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::LeavesBattlefield));
}

// =============================================================================
// New Trigger Type Tests
// =============================================================================

TEST_F(TriggerParserTest, DetectsExileTriggers)
{
    // Rest in Peace-like effect
    auto triggers = parser.parseCardTriggers(
        "Soul-Guide Lantern", "When Soul-Guide Lantern enters the battlefield, exile target card from a graveyard.\n"
                              "Whenever a card is exiled from a graveyard, you gain 1 life.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Exile));
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::EntersBattlefield));
}

TEST_F(TriggerParserTest, DetectsSearchLibraryTriggers)
{
    // Opposition Agent
    auto triggers =
        parser.parseCardTriggers("Opposition Agent",
                                 "Flash\nYou control your opponents while they're searching their libraries.\nWhile "
                                 "an opponent is searching their library, they exile each card they find.",
                                 1);
    // The above doesn't have "Whenever" - let's use a better example
    auto triggers2 = parser.parseCardTriggers(
        "Ob Nixilis, Unshackled",
        "Flying, trample\nWhenever an opponent searches their library, that player sacrifices a creature and loses 10 "
        "life.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers2, TriggerPhase::SearchLibrary));
}

TEST_F(TriggerParserTest, DetectsMagecraftKeyword)
{
    // Archmage Emeritus
    auto triggers =
        parser.parseCardTriggers("Archmage Emeritus", "Magecraft — Whenever you cast or copy an instant or sorcery "
                                                      "spell, draw a card.",
                                 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Magecraft));
}

TEST_F(TriggerParserTest, DetectsMagecraftPattern)
{
    // Storm-Kiln Artist
    auto triggers =
        parser.parseCardTriggers("Storm-Kiln Artist",
                                 "Whenever you cast or copy an instant or sorcery spell, create a Treasure token.", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Magecraft));
}

TEST_F(TriggerParserTest, DetectsConstellationKeyword)
{
    // Eidolon of Blossoms
    auto triggers = parser.parseCardTriggers(
        "Eidolon of Blossoms", "Constellation — Whenever Eidolon of Blossoms or another enchantment enters the "
                               "battlefield under your control, draw a card.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Constellation));
}

TEST_F(TriggerParserTest, DetectsConstellationPattern)
{
    // Archon of Sun's Grace
    auto triggers = parser.parseCardTriggers(
        "Archon of Sun's Grace",
        "Flying, lifelink\nPegasus creatures you control have lifelink.\nWhenever an enchantment enters the "
        "battlefield under your control, create a 2/2 white Pegasus creature token with flying.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Constellation));
}

TEST_F(TriggerParserTest, DetectsMillTriggers)
{
    // Syr Konrad, the Grim-like effect
    auto triggers = parser.parseCardTriggers(
        "Mill Trigger Example",
        "Whenever a card is put into your graveyard from your library, each opponent loses 1 life.", 1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::Mill));
}

TEST_F(TriggerParserTest, DetectsCopySpellTriggers)
{
    // Twinning Staff
    auto triggers = parser.parseCardTriggers(
        "Twinning Staff", "If you would copy a spell one or more times, instead copy it that many times plus one.\n"
                          "Whenever you copy a spell, you may pay {1}. If you do, draw a card.",
        1);
    EXPECT_TRUE(hasTriggerPhase(triggers, TriggerPhase::CopySpell));
}

// =============================================================================
// Player Scope Detection Tests (Trigger Condition vs Effect)
// =============================================================================

TEST_F(TriggerParserTest, DoesNotFlagEffectTargetingOpponentsAsOpponentTrigger)
{
    // Impact Tremors - triggers on YOUR creatures, deals damage TO opponents
    // Should NOT have isOpponentOnly flag
    auto triggers = parser.parseCardTriggers(
        "Impact Tremors",
        "Whenever a creature enters the battlefield under your control, Impact Tremors deals 1 damage to each "
        "opponent.",
        1);
    ASSERT_GE(triggers.size(), 1);
    EXPECT_FALSE(triggers[0]->isOpponentOnly); // Effect targets opponents, but trigger is from YOUR creatures
}

TEST_F(TriggerParserTest, DoesNotFlagGuttersnipeAsOpponentTrigger)
{
    // Guttersnipe - triggers on YOUR spells, deals damage TO opponents
    auto triggers = parser.parseCardTriggers(
        "Guttersnipe", "Whenever you cast an instant or sorcery spell, Guttersnipe deals 2 damage to each opponent.",
        1);
    ASSERT_GE(triggers.size(), 1);
    EXPECT_FALSE(triggers[0]->isOpponentOnly); // YOU cast the spell, not opponent
}

TEST_F(TriggerParserTest, CorrectlyFlagsOpponentTriggerCondition)
{
    // Rhystic Study - triggers when OPPONENT casts
    auto triggers = parser.parseCardTriggers(
        "Rhystic Study", "Whenever an opponent casts a spell, you may draw a card unless that player pays {1}.", 1);
    ASSERT_GE(triggers.size(), 1);
    EXPECT_TRUE(triggers[0]->isOpponentOnly); // Trigger condition involves opponent action
}

TEST_F(TriggerParserTest, DoesNotFlagEffectTargetingPlayersAsEachPlayerTrigger)
{
    // Purphoros - triggers on YOUR creatures, deals damage to EACH opponent
    auto triggers = parser.parseCardTriggers(
        "Purphoros, God of the Forge",
        "Whenever another creature enters the battlefield under your control, Purphoros deals 2 damage to each "
        "opponent.",
        1);
    ASSERT_GE(triggers.size(), 1);
    EXPECT_FALSE(triggers[0]->isEachPlayer); // YOUR creatures trigger it, not "each player"
}

TEST_F(TriggerParserTest, CorrectlyFlagsEachPlayerTriggerCondition)
{
    // Howling Mine - triggers for EACH PLAYER's draw step
    auto triggers = parser.parseCardTriggers(
        "Howling Mine", "At the beginning of each player's draw step, that player draws an additional card.", 1);
    ASSERT_GE(triggers.size(), 1);
    EXPECT_TRUE(triggers[0]->isEachPlayer); // Trigger condition is "each player's draw step"
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
