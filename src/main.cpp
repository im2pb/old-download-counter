#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>

#include "Statistics.hpp"

#include <cmath>

using namespace geode::prelude;

class $modify(OldDownloadCounterCell, LevelCell) {
    static void onModify(auto& self) {
        if (auto result = self.setHookPriorityAfterPost(
                "LevelCell::loadCustomLevelCell", "geode.node-ids"); !result) {
            log::warn("Could not order level-cell hook after Node IDs: {}", result.unwrapErr());
        }
    }

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        if (!m_level || !m_mainLayer) return;

        auto lengthIcon = typeinfo_cast<CCSprite*>(m_mainLayer->getChildByID("length-icon"));
        auto lengthLabel = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("length-label"));
        auto downloadsIcon = typeinfo_cast<CCSprite*>(m_mainLayer->getChildByID("downloads-icon"));
        auto downloadsLabel = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("downloads-label"));
        auto likesIcon = typeinfo_cast<CCSprite*>(m_mainLayer->getChildByID("likes-icon"));
        auto likesLabel = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("likes-label"));

        std::array<CCNode*, 6> row {
            lengthIcon, lengthLabel, downloadsIcon, downloadsLabel, likesIcon, likesLabel
        };
        for (auto node : row) {
            if (
                !node || !node->isVisible() || 
                node->getParent() != m_mainLayer ||     
                node->getRotation() != 0.f || 
                node->getScaleX() <= 0.f || 
                node->getScaleY() <= 0.f
            ) {
                return;
            }
        }

        auto const start = lengthIcon->boundingBox().getMinX();
        float bottom = row.front()->boundingBox().getMinY();
        float top = row.front()->boundingBox().getMaxY();

        for (auto node : row) {
            bottom = std::min(bottom, node->boundingBox().getMinY());
            top = std::max(top, node->boundingBox().getMaxY());
        }

        auto right = m_mainLayer->convertToNodeSpace(convertToWorldSpace({m_width - 8.f, 0.f})).x;
      
        if (m_mainMenu && m_mainMenu->isVisible()) {
            for (auto control : CCArrayExt<CCNode*>(m_mainMenu->getChildren())) {
                if (!control->isVisible()) continue;
                auto const bounds = control->boundingBox();
                auto const low = m_mainLayer->convertToNodeSpace(
                    m_mainMenu->convertToWorldSpace({bounds.getMinX(), bounds.getMinY()}));
                auto const high = m_mainLayer->convertToNodeSpace(
                    m_mainMenu->convertToWorldSpace({bounds.getMaxX(), bounds.getMaxY()}));
                if (high.y > bottom && low.y < top && low.x > start) {
                    right = std::min(right, low.x - 4.f);
                }
            }
        }
        if (!std::isfinite(right - start) || right <= start) return;

        downloadsLabel->setString(old_download_counter::formatCount(m_level->m_downloads).c_str());
        likesLabel->setString(old_download_counter::formatRating(m_level->m_likes).c_str());

        std::array<float, 6> widths;
        for (std::size_t i = 0; i < row.size(); ++i) {
            widths[i] = row[i]->boundingBox().size.width;
        }

        auto const layout = old_download_counter::layoutRow(widths, right - start, m_compactView ? 2.f : 3.f, m_compactView ? 6.f : 8.f);

        for (std::size_t i = 0; i < row.size(); ++i) {
            auto node = row[i];
            auto const centerY = node->boundingBox().getMidY();

            node->setScaleX(node->getScaleX() * layout.scale);
            node->setScaleY(node->getScaleY() * layout.scale);

            auto const bounds = node->boundingBox();
        
            node->setPositionX(node->getPositionX() + start + layout.left[i] - bounds.getMinX());
            node->setPositionY(node->getPositionY() + centerY - bounds.getMidY());
        }

        if (auto icon = typeinfo_cast<CCSprite*>(m_mainLayer->getChildByID("orbs-icon"))) {
            icon->setVisible(false);
        }
        
        if (auto label = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("orbs-label"))) {
            label->setVisible(false);
        }
    }
};
